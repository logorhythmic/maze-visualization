#include "colors.h"
#include "gui.h"
#include "state_manager.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#define SCR_WIDTH 1200
#define SCR_HEIGHT 750

static void Set_SDL_Scaling(SDL_Window *window, SDL_Renderer *renderer);

int main() {

  SDL_Init(SDL_INIT_VIDEO);
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  // float main_scale = 1.0f;
  printf("Main scale is: %f\n", main_scale);

  SDL_Window *window = SDL_CreateWindow("Maze", SCR_WIDTH, SCR_HEIGHT,
                                        SDL_WINDOW_HIGH_PIXEL_DENSITY);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  SDL_SetRenderVSync(renderer, 1);

  // Displaying window dimensions
  int w, h;
  SDL_GetWindowSizeInPixels(window, &w, &h);
  printf("Window width: %d, Window Height: %d\n", w, h);
  //

  SDL_Event event;

  // Delta Time Calculation
  const double frequency_inv = 1.0 / (double)SDL_GetPerformanceFrequency();
  double delta_time_ms = 0.0;
  uint64_t last_start = SDL_GetPerformanceCounter();
  //

  // Setting up state
  State *state = State_Create(renderer);

  // Setting up UI
  GuiInfo *gi = Create_Gui_Info(window, renderer, &event);

  bool done = false;
  while (!done) {
    uint64_t current_start = SDL_GetPerformanceCounter();
    delta_time_ms = (current_start - last_start) * frequency_inv * 1000.0;
    last_start = current_start;
    while (SDL_PollEvent(&event)) {
      Process_Gui_Event(gi);
      switch (event.type) {
      case SDL_EVENT_QUIT:
        done = true;
        break;
      case SDL_EVENT_KEY_DOWN:
        State_Process_Event(state, &event);
        break;
      }
    }

    Draw_Gui_Frame(gi);

    // Setting the screen scaling
    Set_SDL_Scaling(window, renderer);

    // Clearing Screen
    SDL_Color bg = COLOR_RENDER_BACKGROUND;
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    State_Update(state, delta_time_ms);
    State_Render(state);

    Render_Gui_Frame(gi);

    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  State_Destroy(state);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

static void Set_SDL_Scaling(SDL_Window *window, SDL_Renderer *renderer) {
  int logical_w, logical_h;
  int pixel_w, pixel_h;
  SDL_GetWindowSize(window, &logical_w, &logical_h);
  SDL_GetWindowSizeInPixels(window, &pixel_w, &pixel_h);

  float scale_x = (float)pixel_w / (float)logical_w;
  float scale_y = (float)pixel_h / (float)logical_h;

  SDL_SetRenderScale(renderer, scale_x, scale_y);
}
