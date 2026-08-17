#pragma once
#include "state_manager.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GUI_WIDTH 300

void GUI_Init(SDL_Window *window, SDL_Renderer *renderer);

void GUI_Process_Event(SDL_Event *event);

void GUI_Draw_Frame(MazeUIState *ui_state, MazeContext *ctx);

void GUI_Render_Frame(SDL_Renderer *renderer);

void GUI_Deinit();

#ifdef __cplusplus
}
#endif
