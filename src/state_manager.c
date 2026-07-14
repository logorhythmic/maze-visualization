#include "../include/state_manager.h"
#include "../include/colors.h"
#include "../include/generation.h"
#include "../include/maze.h"
#include "../include/maze_render.h"
#include <stdio.h>
#include <stdlib.h>

#define CELL_SIZE 100
#define MAZE_START_POS ((Vector2){45, 45})
#define COLUMNS 8
#define ROWS 8
#define WALL_THICKNESS 5

#define DFS_TIME_DELAY_MS 100

typedef enum { MODE_NONE, DFS_GEN, DFS_SOLVE, BFS_SOLVE, ASTAR_SOLVE } MazeMode;

struct State {
  Maze *maze;
  MazeRender *maze_render;

  MazeEvents *generate_events;
  MazeEvents *solve_events;
  MazeEvents *current_events;

  bool maze_is_blank;

  double time_elapsed;

  SDL_Renderer *renderer;

  MazeMode maze_mode;
  RenderStyle render_style;
};

State *State_Create(SDL_Renderer *renderer) {
  State *state = calloc(1, sizeof(State));
  CellPos start_cell = {5, 5};
  CellPos end_cell = {9, 9};
  Maze *maze = Maze_Create(ROWS, COLUMNS, start_cell, end_cell);
  state->maze = maze;
  if (state->maze == NULL) {
    printf("Maze failed to allocate. Exiting");
    exit(1);
  }

  MazeRender *maze_render = Maze_Render_Create(
      renderer, CELL_SIZE, MAZE_START_POS, WALL_THICKNESS, COLOR_BLACK);
  state->maze_render = maze_render;
  if (state->maze_render == NULL) {
    printf("Maze Render failed to allocate. Exiting");
    exit(1);
  }

  state->maze_mode = MODE_NONE;
  state->render_style = RENDER_STATIC;
  state->current_events = NULL;
  state->solve_events = NULL;
  state->generate_events = NULL;
  state->maze_is_blank = true;
  state->time_elapsed = 0.0;
  return state;
}

void State_Set_MazeMode(const SDL_Event *event, State *state) {

  if (event->key.key == SDLK_S) {
    printf("Changed to Render static mode\n");

    state->render_style = RENDER_STATIC;

    // Step through all events only if MazeEvents have been generated
    if (state->current_events != NULL) {
      Maze_StepAll_Event(state->current_events, state->maze);
    }
  }
  if (event->key.key == SDLK_A) {
    // Safety check
    if (state->current_events == NULL) {
      state->render_style = RENDER_STATIC;
      return;
    }

    printf("Render Animated mode\n");
    Maze_Reset(state->maze);
    state->render_style = RENDER_ANIMATED;
  }

  if (event->key.key == SDLK_D) {
    state->generate_events = DFSGen_Generate_MazeEvents(state->maze);
    state->current_events = state->generate_events;
    state->render_style = RENDER_STATIC;
    state->maze_mode = DFS_GEN;
  }
}

void State_Render(State *state_info, uint64_t delta_time_ms) {

  SDL_Renderer *renderer = state_info->renderer;
  switch (state_info->render_style) {

  case RENDER_STATIC:
    Maze_Render_Draw(state_info->maze_render, state_info->maze);
    break;

  case RENDER_ANIMATED:

    state_info->time_elapsed += delta_time_ms;

    while (state_info->time_elapsed >= DFS_TIME_DELAY_MS) {
      state_info->time_elapsed -= DFS_TIME_DELAY_MS;
      if (!Maze_Step_Event(state_info->current_events, state_info->maze)) {
        state_info->render_style = RENDER_STATIC;
        break;
      }
    }

    Maze_Render_Draw(state_info->maze_render, state_info->maze);

    break;
  }
}

void State_Destroy(State *state) {
  Maze_Destroy_Events(state->generate_events);
  Maze_Destroy_Events(state->solve_events);
  Maze_Render_Destroy(state->maze_render);
  Maze_Destroy(state->maze);
  free(state);
}
