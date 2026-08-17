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
  mu->animate = true;
  mu->skipRequest = false;
  mu->rows = DEFAULT_ROWS;
  mu->columns = DEFAULT_COLUMNS;
  mu->start_pos[0] = 0;
  mu->start_pos[1] = 0;
  mu->end_pos[0] = DEFAULT_COLUMNS - 1;
  mu->end_pos[1] = DEFAULT_ROWS - 1;
  mu->speed = DEFAULT_SPEED;
  mu->time_delay = TIME_DELAY_MS;
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

void MazeContext_Set_MazeDimensions(MazeContext *ctx) {
  ctx->maze_ui_state->maze_mode = MAZE_BLANK;
  Maze_Set_Dimensions(ctx->maze, ctx->maze_ui_state->rows,
                      ctx->maze_ui_state->columns);
}

void MazeContext_Set_MazeEndpoints(MazeContext *ctx) {
  CellPos start = {ctx->maze_ui_state->start_pos[0],
                   ctx->maze_ui_state->start_pos[1]};
  CellPos end = {ctx->maze_ui_state->end_pos[0],
                 ctx->maze_ui_state->end_pos[1]};

  Maze_Set_Endpoints(ctx->maze, start, end);

  if (ctx->maze_ui_state->maze_mode == MAZE_SOLVED) {
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

void Event_DFSGen(MazeContext *state) {
  MazeEvents *DFSGen_events = DFSGen_Generate_MazeEvents(state->maze);

  // Destroy previous events if exist (To prevent memory leaks)
  if (state->current_events != NULL) {
    MazeEvents_Destroy(state->current_events);
  }

  // Current events are now populated with DFSGen_Events
  state->current_events = DFSGen_events;

  // Set MazeMode to generation
  state->maze_ui_state->maze_mode = MAZE_GENERATING;

  // Need to always reset maze to clear previous generation or solving (if any)
  Maze_Reset(state->maze);

  // If not being animated, display final Maze
  if (!state->maze_ui_state->animate) {
    MazeEvents_StepAll(state->current_events, state->maze);
    Maze_SetAll_CellState(state->maze, STATE_GENERATED);
    state->maze_ui_state->maze_mode = MAZE_GENERATED;
    printf("Setting maze as genenrated");
  }
  printf("Maze Mode: %d\n", state->maze_ui_state->maze_mode);
}

void Event_PrimsGen(MazeContext *state) {
  MazeEvents *PrimsGen_events = PrimsGen_Generate_MazeEvents(state->maze);

  // Destroy previous events if exist (To prevent memory leaks)
  if (state->current_events != NULL) {
    MazeEvents_Destroy(state->current_events);
  }

  // Current events are now populated with PrimsGen_Events
  state->current_events = PrimsGen_events;

  // Set MazeMode to generation
  state->maze_ui_state->maze_mode = MAZE_GENERATING;

  // Need to always reset maze to clear previous generation or solving (if any)
  Maze_Reset(state->maze);

  // If not being animated, display final Maze
  if (!state->maze_ui_state->animate) {
    MazeEvents_StepAll(state->current_events, state->maze);
    Maze_SetAll_CellState(state->maze, STATE_GENERATED);
    state->maze_ui_state->maze_mode = MAZE_GENERATED;
  }
}

void Event_DFSSolve(MazeContext *state) {
  printf("Maze Mode is: %d\n", state->maze_ui_state->maze_mode);
  if (state->maze_ui_state->maze_mode != MAZE_GENERATED &&
      state->maze_ui_state->maze_mode != MAZE_SOLVED) {
    printf("Maze has not been generated. Generate the maze first\n");
    return;
  }

  // Destroy previous events if exist (To prevent memory leaks)
  if (state->current_events != NULL) {
    MazeEvents_Destroy(state->current_events);
  }

  printf("Using DFS Solve now\n");

  // Need to reset all Cell fills
  Maze_SetAll_CellState(state->maze, STATE_GENERATED);

  MazeEvents *DFSSolve_Events = DFSSolve_Generate_MazeEvents(state->maze);
  state->current_events = DFSSolve_Events;

  // Set MazeMode to solving
  state->maze_ui_state->maze_mode = MAZE_SOLVING;

  // If not being animated, display the final solution
  if (!state->maze_ui_state->animate) {
    MazeEvents_StepAll(state->current_events, state->maze);
    state->maze_ui_state->maze_mode = MAZE_SOLVED;
  }
}

void Event_BFSSolve(MazeContext *state) {

  if (state->maze_ui_state->maze_mode != MAZE_GENERATED &&
      state->maze_ui_state->maze_mode != MAZE_SOLVED) {
    printf("Maze has not been generated. Generate the maze first\n");
    return;
  }

  // Destroy previous events if exist (To prevent memory leaks)
  if (state->current_events != NULL) {
    MazeEvents_Destroy(state->current_events);
  }

  printf("Using BFS Solve now\n");

  // Need to reset all Cell fills
  Maze_SetAll_CellState(state->maze, STATE_GENERATED);

  MazeEvents *BFSSolve_Events = BFSSolve_Generate_MazeEvents(state->maze);
  state->current_events = BFSSolve_Events;

  // Set MazeMode to solving
  state->maze_ui_state->maze_mode = MAZE_SOLVING;

  // If not being animated, display the final solution
  if (!state->maze_ui_state->animate) {
    MazeEvents_StepAll(state->current_events, state->maze);
    state->maze_ui_state->maze_mode = MAZE_SOLVED;
  }
}

void MazeContext_Process_Event(MazeContext *state, const SDL_Event *event) {

  //------------------------------ Key Pressess-------------------------------
  switch (event->key.key) {

  case SDLK_A: // Animation Toggle
    state->maze_ui_state->animate = !state->maze_ui_state->animate;
    printf("Animation is turned %s\n",
           state->maze_ui_state->animate ? "on" : "off");
    break;

  case SDLK_D: // Init DFS Generation
    Event_DFSGen(state);
    break;

  case SDLK_P:
    Event_PrimsGen(state);
    break;

  case SDLK_1:
    Event_DFSSolve(state);
    break;

  case SDLK_2:
    Event_BFSSolve(state);
    break;
  }
  //---------------------------------------------------------------------------

  //
}

void MazeContext_Update(MazeContext *ctx, uint64_t delta_time_ms) {

  assert(ctx != NULL);
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

  if (ctx->maze_ui_state->skipRequest) {
    if (ctx->maze_ui_state->maze_mode == MAZE_GENERATING ||
        ctx->maze_ui_state->maze_mode == MAZE_SOLVING) {
      MazeEvents_StepAll(ctx->current_events, ctx->maze);
      printf("Animation turned off, stepped through all events\n");
      ctx->maze_ui_state->maze_mode = MAZE_GENERATED;
      if (ctx->maze_ui_state->maze_mode == MAZE_SOLVING) {
        ctx->maze_ui_state->maze_mode = MAZE_SOLVED;
      }
    }
    ctx->maze_ui_state->skipRequest = false;
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
