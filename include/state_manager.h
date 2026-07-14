#pragma once
#include "maze.h"
#include "maze_render.h"

typedef struct State State;

typedef enum {
  RENDER_STATIC,
  RENDER_ANIMATED,
} RenderStyle;

State *State_Create(SDL_Renderer *renderer);

void State_Set_MazeMode(const SDL_Event *event, State *state);

void State_Render(State *state_info, uint64_t delta_time_ms);

void State_Destroy(State *state);
