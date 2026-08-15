#pragma once
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#define DEFAULT_COLUMNS 10
#define DEFAULT_ROWS 10

#define MAX_ROWS 100
#define MAX_COLUMNS 100

#define MIN_ROWS 5
#define MIN_COLUMNS 5

#ifdef __cplusplus

extern "C" {

#endif

typedef struct State State;

State *State_Create(SDL_Renderer *renderer);

void State_Process_Event(State *state, const SDL_Event *event);

void State_Set_MazeDimensions(State *state, int row, int columns);

void State_Set_MazeEndpoints(State *state, int start_x, int start_y, int end_x,
                             int end_y);

void State_Render(State *state_info);

void State_Update(State *state, uint64_t delta_time_ms);

void State_Destroy(State *state);

#ifdef __cplusplus
}
#endif
