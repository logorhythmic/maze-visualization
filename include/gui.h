#pragma once
#include "state_manager.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GUI_WIDTH 300
typedef struct GuiInfo GuiInfo;
GuiInfo *Create_Gui_Info(SDL_Window *window, SDL_Renderer *renderer,
                         SDL_Event *event);

void Process_Gui_Event(GuiInfo *gi);

void Draw_Gui_Frame(State *state, GuiInfo *gi);

void Render_Gui_Frame(GuiInfo *gi);

void Destroy_Gui(GuiInfo *gi);

#ifdef __cplusplus
}
#endif
