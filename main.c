#include "include/colors.h"
#include "include/maze.h"
#include "include/maze_render.h"
#include "include/state_manager.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#define SCR_WIDTH 900
#define SCR_HEIGHT 900

int main() {
  SDL_SetHint(SDL_HINT_VIDEO_WAYLAND_SCALE_TO_DISPLAY, "1");
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window = SDL_CreateWindow("Maze", SCR_WIDTH, SCR_HEIGHT, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  SDL_SetRenderVSync(renderer, 1);
  int w, h;
  SDL_GetWindowSizeInPixels(window, &w, &h);
  printf("Window width: %d, Window Height: %d\n", w, h);
  SDL_Event event;

  SUBMODE_DFS_INFO *sdi = DFSGen_Create_Submode(renderer);

  const double frequency_inv = 1.0 / (double)SDL_GetPerformanceFrequency();
  double delta_time_ms = 0.0;
  uint64_t last_start = SDL_GetPerformanceCounter();

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

        if (event.key.key == SDLK_S) {
          printf("Displaying maze static\n");
          DFS_Set_Submode(sdi, DISPLAY_MAZE_STATIC);
        }

        if (event.key.key == SDLK_V) {
          printf("Starting Visualization\n");
          DFS_Set_Submode(sdi, DISPLAY_VISUALIZE);
        }
        break;
      }
    }

    SDL_Color bg = COLOR_RENDER_BACKGROUND;
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);
    DFS_Process_Submode(renderer, sdi, delta_time_ms);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  DFSGen_Destroy_Submode(sdi);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
