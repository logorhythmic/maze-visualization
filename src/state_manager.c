#include "state_manager.h"
#include "colors.h"
#include "generation.h"
#include "gui.h"
#include "maze.h"
#include "maze_render.h"
#include "solving.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define TIME_DELAY_MS 100

#define WALL_THICKNESS 1
#define SOLN_LINE_THICK 0.45f

struct MazeContext {
  Maze *maze;
  MazeRender *maze_render;
  MazeEvents *current_events;
  MazeUIState *maze_ui_state;

  double time_elapsed;
};

MazeUIState *MazeUIState_Create() {
  MazeUIState *mu = calloc(1, sizeof(MazeUIState));
  *mu = (MazeUIState){

      .animate = true,
      .skipRequest = false,

      .rows = DEFAULT_ROWS,
      .columns = DEFAULT_COLUMNS,

      .start_pos[0] = 0,
      .start_pos[1] = 0,

      .end_pos[0] = DEFAULT_COLUMNS - 1,
      .end_pos[1] = DEFAULT_ROWS - 1,

      .speed = DEFAULT_SPEED,
      .time_delay = TIME_DELAY_MS,

      .gen_algo = 0,
      .gen_algo_names = {"Randomized DFS", "Randomized Prims",
                         "Randomized Kruskals"},

      .solve_algo = 0,
      .solve_algo_names = {"Depth First Search", "Breadth First Search",
                           "AStar"},
  };

  return mu;
}

MazeContext *MazeContext_Create(MazeUIState *mu, SDL_Renderer *renderer) {
  MazeContext *state = calloc(1, sizeof(MazeContext));
  state->maze_ui_state = mu;

  CellPos start_cell = {0, 0};
  CellPos end_cell = {DEFAULT_ROWS - 1, DEFAULT_COLUMNS - 1};

  state->maze = Maze_Create();
  Maze_Set_Dimensions(state->maze, DEFAULT_ROWS, DEFAULT_COLUMNS);
  Maze_Set_Endpoints(state->maze, start_cell, end_cell);

  int w, h;
  SDL_Window *window = SDL_GetRenderWindow(renderer);
  SDL_GetWindowSize(window, &w, &h); // 900, 900

  Vector2 view_dimensions = {w - GUI_WIDTH, h};
  Vector2 view_padding = {20, 20};
  state->maze_render =
      Maze_Render_Create(view_dimensions, view_padding, WALL_THICKNESS,
                         COL_WALL, SOLN_LINE_THICK, COL_STATE_GENERATED);
  state->current_events = NULL;

  double time_elapsed = 0;

  return state;
}

void MazeContext_Event_SetMazeDimensions(MazeContext *ctx) {
  ctx->maze_ui_state->maze_mode = MAZE_BLANK;
  Maze_Set_Dimensions(ctx->maze, ctx->maze_ui_state->rows,
                      ctx->maze_ui_state->columns);
}

void MazeContext_Event_SetMazeEndpoints(MazeContext *ctx) {
  CellPos start = {ctx->maze_ui_state->start_pos[0],
                   ctx->maze_ui_state->start_pos[1]};
  CellPos end = {ctx->maze_ui_state->end_pos[0],
                 ctx->maze_ui_state->end_pos[1]};

  Maze_Set_Endpoints(ctx->maze, start, end);

  if (ctx->maze_ui_state->maze_mode == MAZE_SOLVED ||
      ctx->maze_ui_state->maze_mode == MAZE_SOLVING) {
    Maze_SetAll_CellState(ctx->maze, STATE_GENERATED);
    ctx->maze_ui_state->maze_mode = MAZE_GENERATED;
    return;
  }

  if (ctx->maze_ui_state->maze_mode != MAZE_GENERATED) {
    printf("Maze not generated. Current Maze Mode: %d\n",
           ctx->maze_ui_state->maze_mode);
    Maze_Reset(ctx->maze);
    ctx->maze_ui_state->maze_mode = MAZE_BLANK;
  }
}

void MazeContext_Event_ResetMaze(MazeContext *ctx) {
  Maze_Reset(ctx->maze);
  ctx->maze_ui_state->maze_mode = MAZE_BLANK;
}

void MazeContext_Event_ClearSolution(MazeContext *ctx) {
  Maze_SetAll_CellState(ctx->maze, STATE_GENERATED);
  ctx->maze_ui_state->maze_mode = MAZE_GENERATED;
}

void MazeContext_Event_SkipAnimation(MazeContext *ctx) {
  if (ctx->maze_ui_state->maze_mode == MAZE_GENERATING ||
      ctx->maze_ui_state->maze_mode == MAZE_SOLVING) {
    MazeEvents_StepAll(ctx->current_events, ctx->maze);
    printf("Animation turned off, stepped through all events\n");
    ctx->maze_ui_state->maze_mode = MAZE_GENERATED;
    if (ctx->maze_ui_state->maze_mode == MAZE_SOLVING) {
      ctx->maze_ui_state->maze_mode = MAZE_SOLVED;
    }
  }
}

void MazeContext_Event_GenerateMaze(MazeContext *ctx) {
  // Destroy previous events if exist (To prevent memory leaks)
  if (ctx->current_events != NULL) {
    MazeEvents_Destroy(ctx->current_events);
    ctx->current_events = NULL;
  }

  MazeEvents *events;
  switch (ctx->maze_ui_state->gen_algo) {
  case GEN_DFS:
    events = DFSGen_Generate_MazeEvents(ctx->maze);
    break;

  case GEN_PRIMS:
    events = PrimsGen_Generate_MazeEvents(ctx->maze);
    break;

  case GEN_KRUSKAL:
    ctx->maze_ui_state->maze_mode = MAZE_BLANK;
    return;
    break;

  case TOTAL_GEN_ALGO:
    return;
    break;
  }
  ctx->current_events = events;

  // Set MazeMode to generation
  ctx->maze_ui_state->maze_mode = MAZE_GENERATING;

  // Need to always reset maze to clear previous generation or solving (if any)
  Maze_Reset(ctx->maze);

  if (!ctx->maze_ui_state->animate) {
    MazeEvents_StepAll(ctx->current_events, ctx->maze);
    Maze_SetAll_CellState(ctx->maze, STATE_GENERATED);
    ctx->maze_ui_state->maze_mode = MAZE_GENERATED;
    printf("Maze Generated");
  }
  printf("Maze Mode: %d\n", ctx->maze_ui_state->maze_mode);
}

void MazeContext_Event_SolveMaze(MazeContext *ctx) {

  // Destroy previous events if exist (To prevent memory leaks)
  if (ctx->current_events != NULL) {
    MazeEvents_Destroy(ctx->current_events);
    ctx->current_events = NULL;
  }

  MazeEvents *events;
  switch (ctx->maze_ui_state->solve_algo) {
  case SOLVE_DFS:
    events = DFSSolve_Generate_MazeEvents(ctx->maze);
    break;

  case SOLVE_BFS:
    events = BFSSolve_Generate_MazeEvents(ctx->maze);
    break;

  case SOLVE_ASTAR:
    ctx->maze_ui_state->maze_mode = MAZE_GENERATED;
    return;
    break;

  case TOTAL_GEN_ALGO:
    return;
    break;
  }
  ctx->current_events = events;

  // Set MazeMode to generation
  ctx->maze_ui_state->maze_mode = MAZE_SOLVING;

  // Need to reset all Cell fills
  Maze_SetAll_CellState(ctx->maze, STATE_GENERATED);

  if (!ctx->maze_ui_state->animate) {
    MazeEvents_StepAll(ctx->current_events, ctx->maze);
    ctx->maze_ui_state->maze_mode = MAZE_SOLVED;
    printf("Maze Solved");
  }
  printf("Maze Mode: %d\n", ctx->maze_ui_state->maze_mode);
}

void MazeContext_Update(MazeContext *ctx, uint64_t delta_time_ms) {

  assert(ctx != NULL);

  assert(!(ctx->current_events == NULL &&
           (ctx->maze_ui_state->maze_mode == MAZE_GENERATING ||
            ctx->maze_ui_state->maze_mode == MAZE_SOLVING)));

  assert(ctx->maze_ui_state != NULL);

  if (ctx->maze_ui_state->maze_mode == MAZE_GENERATED ||
      ctx->maze_ui_state->maze_mode == MAZE_BLANK) {
    return;
  }

  if (ctx->maze_ui_state->animate) {
    if (ctx->maze_ui_state->maze_mode == MAZE_GENERATING ||
        ctx->maze_ui_state->maze_mode == MAZE_SOLVING) {
      ctx->time_elapsed += delta_time_ms;

      while (ctx->time_elapsed >= ctx->maze_ui_state->time_delay) {
        ctx->time_elapsed -= ctx->maze_ui_state->time_delay;

        if (!MazeEvents_Step(ctx->current_events, ctx->maze)) {
          printf("Animation finished\n");
          if (ctx->maze_ui_state->maze_mode == MAZE_GENERATING) {
            ctx->maze_ui_state->maze_mode = MAZE_GENERATED;
          } else {
            ctx->maze_ui_state->maze_mode = MAZE_SOLVED;
          }
          break;
        }
      }
    }
  }
}

void MazeContext_Render(SDL_Renderer *r, MazeContext *state) {

  Maze_Render(r, state->maze_render, state->maze);
}

void MazeContext_Destroy(MazeContext *state) {
  free(state->maze_ui_state);
  Maze_Render_Destroy(state->maze_render);
  MazeEvents_Destroy(state->current_events);
  Maze_Destroy(state->maze);
  free(state);
}
