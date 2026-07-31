#pragma once
#include "maze.h"
#include "maze_render.h"
#include <SDL3/SDL_events.h>

typedef struct State State;

State *State_Create(SDL_Renderer *renderer);

void State_Process_Event(State *state, const SDL_Event *event);

void State_Render(State *state_info);

void State_Update(State *state, uint64_t delta_time_ms);

void State_Destroy(State *state);
