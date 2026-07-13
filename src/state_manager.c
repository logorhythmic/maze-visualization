#include "../include/state_manager.h"
#include "../include/colors.h"
#include "../include/generation.h"
#include <stdio.h>
#include <stdlib.h>

#define CELL_SIZE 100
#define MAZE_START_POS ((Vector2){45, 45})
#define COLUMNS 8
#define ROWS 8
#define WALL_THICKNESS 5

#define DFS_TIME_DELAY_MS 100

struct SUBMODE_DFS_INFO {
  Maze *maze;
  MazeRender *maze_render;
  MazeEvents *DFSGen_events;
  bool dfs_gen_finished;

  bool maze_is_blank;

  double time_elapsed;

  SUBMODE current_submode;
};

SUBMODE_DFS_INFO *DFSGen_Create_Submode(SDL_Renderer *renderer) {
  SUBMODE_DFS_INFO *sdi = calloc(1, sizeof(SUBMODE_DFS_INFO));
  CellPos start_cell = {5, 5};
  CellPos end_cell = {9, 9};
  Maze *maze = Maze_Create(ROWS, COLUMNS, start_cell, end_cell);
  MazeRender *maze_render = Maze_Render_Create(
      renderer, CELL_SIZE, MAZE_START_POS, WALL_THICKNESS, COLOR_BLACK);
  sdi->maze = maze;

  if (sdi->maze == NULL) {
    printf("Maze failed to allocate. Exiting");
    exit(1);
  }
  sdi->DFSGen_events = DFSGen_Generate_MazeEvents(maze);
  sdi->maze_render = maze_render;
  sdi->current_submode = IDLE;
  sdi->maze_is_blank = true;
  sdi->time_elapsed = 0.0;
  return sdi;
}

void DFS_Set_Submode(SUBMODE_DFS_INFO *sdi, SUBMODE new_state) {

  if (new_state == DISPLAY_MAZE_STATIC) {
    Maze_Reset(sdi->maze);
    Maze_StepAll_Event(sdi->DFSGen_events, sdi->maze);
    printf("MAZE STATIC, RESTEPPED THROUGH ALL EVENTS\n");
    sdi->current_submode = new_state;
  }

  if (new_state == DISPLAY_VISUALIZE) {
    Maze_Reset(sdi->maze);
    Maze_Reset_EventNumber(sdi->DFSGen_events);
    sdi->current_submode = new_state;
  }

  if (new_state == IDLE) {
    sdi->current_submode = new_state;
  }
}

void DFS_Process_Submode(SDL_Renderer *r, SUBMODE_DFS_INFO *sdi,
                         uint64_t delta_time_ms) {
  switch (sdi->current_submode) {

  case IDLE:
    break;

  case DISPLAY_MAZE_STATIC:
    Maze_Render_Draw(sdi->maze_render, sdi->maze);
    break;

  case DISPLAY_VISUALIZE:
    sdi->time_elapsed += delta_time_ms;
    while (sdi->time_elapsed >= DFS_TIME_DELAY_MS) {
      sdi->time_elapsed -= DFS_TIME_DELAY_MS;
      if (!Maze_Step_Event(sdi->DFSGen_events, sdi->maze)) {
        DFS_Set_Submode(sdi, DISPLAY_MAZE_STATIC);
        break;
      }
    }
    Maze_Render_Draw(sdi->maze_render, sdi->maze);

    break;
  }
}

void DFSGen_Destroy_Submode(SUBMODE_DFS_INFO *sdi) {
  Maze_Render_Destroy(sdi->maze_render);
  Maze_Destroy_Events(sdi->DFSGen_events);
  Maze_Destroy(sdi->maze);
  free(sdi);
}
