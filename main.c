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

  // Setting up MazeUIState and MazeContext
  MazeUIState *maze_ui_state = MazeUIState_Create();
  MazeContext *maze_context = MazeContext_Create(maze_ui_state, renderer);

  // Setting up UI
  GUI_Init(window, renderer);

  bool done = false;
  while (!done) {
    uint64_t current_start = SDL_GetPerformanceCounter();
    delta_time_ms = (current_start - last_start) * frequency_inv * 1000.0;
    last_start = current_start;
    while (SDL_PollEvent(&event)) {

      GUI_Process_Event(&event);

      switch (event.type) {
      case SDL_EVENT_QUIT:
        done = true;
        break;
      case SDL_EVENT_KEY_DOWN:
        break;
      }
    }

    GUI_Draw_Frame(maze_ui_state, maze_context);

    // Setting the screen scaling
    Set_SDL_Scaling(window, renderer);

    // Clearing Screen
    SDL_Color bg = MazeContext_Get_BackgroundColor(maze_context);
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderClear(renderer);

    MazeContext_Update(maze_context, delta_time_ms);
    MazeContext_Render(renderer, maze_context);

    GUI_Render_Frame(renderer);

    SDL_RenderPresent(renderer);
  }

  SDL_DestroyRenderer(renderer);
  MazeContext_Destroy(maze_context);
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
