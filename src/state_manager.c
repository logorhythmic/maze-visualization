#include "../include/state_manager.h"
#include "../include/colors.h"
#include "../include/generation.h"
#include "../include/maze.h"
#include "../include/maze_render.h"
#include "../include/solving.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>
#include <stdlib.h>

#define CELL_SIZE 25
#define MAZE_START_POS ((Vector2){45, 45})
#define COLUMNS 32
#define ROWS 32
#define WALL_THICKNESS 4

typedef enum { MODE_NONE, MODE_GENERATING, MAZE_READY, MODE_SOLVING } MazeMode;

struct State {
  Maze *maze;
  MazeRender *maze_render;

  MazeEvents *current_events;

  bool animate;

  bool maze_is_blank;

  double time_elapsed;
  double time_delay;

  SDL_Renderer *renderer;

  MazeMode maze_mode;
};

void Set_Maze_Ends(Maze *maze) {

};

State *State_Create(SDL_Renderer *renderer, double time_delay_ms) {
  State *state = calloc(1, sizeof(State));
  // CellPos start_cell = {SDL_rand(ROWS), SDL_rand(COLUMNS)};
  // CellPos end_cell = {SDL_rand(ROWS), SDL_rand(COLUMNS)};

  CellPos start_cell = {0, 0};
  CellPos end_cell = {ROWS - 1, COLUMNS - 1};

  state->maze = Maze_Create(ROWS, COLUMNS, start_cell, end_cell);
  state->maze_render = Maze_Render_Create(renderer, CELL_SIZE, MAZE_START_POS,
                                          WALL_THICKNESS, COL_WALL);
  state->maze_mode = MODE_NONE;
  state->time_delay = time_delay_ms;
  state->current_events = NULL;
  return state;
}

void Event_DFSGen(State *state) {
  MazeEvents *DFSGen_events = DFSGen_Generate_MazeEvents(state->maze);

  // Destroy previous events if exist (To prevent memory leaks)
  if (state->current_events != NULL) {
    MazeEvents_Destroy(state->current_events);
  }

  // Current events are now populated with DFSGen_Events
  state->current_events = DFSGen_events;

  // Set MazeMode to generation
  state->maze_mode = MODE_GENERATING;

  // Need to always reset maze to clear previous generation or solving (if any)
  Maze_Reset(state->maze);

  // If not being animated, display final Maze
  if (!state->animate) {
    MazeEvents_StepAll(state->current_events, state->maze);
    state->maze_mode = MAZE_READY;
  }
}

void Event_DFSSolve(State *state) {
  if (state->maze_mode != MAZE_READY) {
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
  state->maze_mode = MODE_SOLVING;

  // If not being animated, display the final solution
  if (!state->animate) {
    MazeEvents_StepAll(state->current_events, state->maze);
    state->maze_mode = MAZE_READY;
  }
}

void Event_BFSSolve(State *state) {

  if (state->maze_mode != MAZE_READY) {
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
  state->maze_mode = MODE_SOLVING;

  // If not being animated, display the final solution
  if (!state->animate) {
    MazeEvents_StepAll(state->current_events, state->maze);
    state->maze_mode = MAZE_READY;
  }
}

void State_Process_Event(State *state, const SDL_Event *event) {

  //------------------------------ Key Pressess-------------------------------
  switch (event->key.key) {

  case SDLK_A: // Animation Toggle
    state->animate = !state->animate;
    printf("Animation is turned %s\n", state->animate ? "on" : "off");
    break;

  case SDLK_D: // Init DFS Generation
    Event_DFSGen(state);
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

void State_Update(State *state, uint64_t delta_time_ms) {

  if (state->maze_mode == MAZE_READY || state->maze_mode == MODE_NONE) {
    return;
  }

  if (state->animate) {
    if (state->maze_mode == MODE_GENERATING ||
        state->maze_mode == MODE_SOLVING) {
      state->time_elapsed += delta_time_ms;

      while (state->time_elapsed >= state->time_delay) {
        state->time_elapsed -= state->time_delay;

        if (!MazeEvents_Step(state->current_events, state->maze)) {
          printf("Animation finished\n");
          state->maze_mode = MAZE_READY;
          break;
        }
      }
    }
  } else {
    if (state->maze_mode == MODE_GENERATING ||
        state->maze_mode == MODE_SOLVING) {
      MazeEvents_StepAll(state->current_events, state->maze);
      printf("Animation turned off, stepped through all events\n");
      state->maze_mode = MAZE_READY;
    }
  }
}

void State_Render(State *state) {

  Maze_Render(state->maze_render, state->maze);
}

void State_Destroy(State *state) {
  Maze_Render_Destroy(state->maze_render);
  MazeEvents_Destroy(state->current_events);
  Maze_Destroy(state->maze);
  free(state);
}
