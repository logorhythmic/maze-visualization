#include "include/colors.h"
#include "include/state_manager.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#define SCR_WIDTH 900
#define SCR_HEIGHT 900

#define TIME_DELAY_MS 10

int main() {

  SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_SCALE_TO_DISPLAY, "1");
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("Maze", SCR_WIDTH, SCR_HEIGHT, 0);
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
  State *state = State_Create(renderer, TIME_DELAY_MS);

  bool done = false;
  while (!done) {
    uint64_t current_start = SDL_GetPerformanceCounter();
    delta_time_ms = (current_start - last_start) * frequency_inv * 1000.0;
    last_start = current_start;
    while (SDL_PollEvent(&event)) {

      switch (event.type) {
      case SDL_EVENT_QUIT:
        done = true;
        break;
      case SDL_EVENT_KEY_DOWN:
        State_Process_Event(state, &event);
        break;
      }
    }
    SDL_Color bg = COLOR_RENDER_BACKGROUND;
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);
    State_Update(state, delta_time_ms);
    State_Render(state);
    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  State_Destroy(state);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
