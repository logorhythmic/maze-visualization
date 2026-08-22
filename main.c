
#define USE_CALLBACK

#ifdef USE_CALLBACK
#define SDL_MAIN_USE_CALLBACKS 1
#endif

#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#include "gui.h"
#include "state_manager.h"

static void Set_SDL_Scaling(SDL_Window *window, SDL_Renderer *renderer) {
  int logical_w, logical_h;
  int pixel_w, pixel_h;
  SDL_GetWindowSize(window, &logical_w, &logical_h);
  SDL_GetWindowSizeInPixels(window, &pixel_w, &pixel_h);

  float scale_x = (float)pixel_w / (float)logical_w;
  float scale_y = (float)pixel_h / (float)logical_h;

  SDL_SetRenderScale(renderer, scale_x, scale_y);
}

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
  MazeUIState *maze_ui_state;
  MazeContext *maze_ctx;

  double freq_inv;
  uint64_t last_start;

} AppState;

#define SCR_WIDTH 1200
#define SCR_HEIGHT 750

#ifdef USE_CALLBACK
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    return SDL_APP_FAILURE;
  }

  AppState *ap = SDL_calloc(1, sizeof(AppState));

  SDL_WindowFlags window_flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;

  if (!SDL_CreateWindowAndRenderer("Maze Visualization", SCR_WIDTH, SCR_HEIGHT,
                                   window_flags, &ap->window, &ap->renderer)) {
    return SDL_APP_FAILURE;
  }

  SDL_SetRenderVSync(ap->renderer, 1);

  int w, h;
  SDL_GetWindowSizeInPixels(ap->window, &w, &h);
  printf("Window width: %d, Window Height: %d\n", w, h);

  // Setting up MazeUIState and MazeContext
  ap->maze_ui_state = MazeUIState_Create();
  ap->maze_ctx = MazeContext_Create(ap->maze_ui_state, ap->renderer);

  // Setting up UI
  GUI_Init(ap->window, ap->renderer);

  // Initing delta time calculation
  ap->freq_inv = 1.0 / (double)SDL_GetPerformanceFrequency();
  ap->last_start = SDL_GetPerformanceFrequency();

  *appstate = ap;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  GUI_Process_Event(event);
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  AppState *as = (AppState *)appstate;

  uint64_t current_start = SDL_GetPerformanceCounter();
  uint64_t delta_time_ms =
      (current_start - as->last_start) * as->freq_inv * 1000.0;
  as->last_start = current_start;

  // Setting the screen scaling
  Set_SDL_Scaling(as->window, as->renderer);

  // Clearing Screen
  SDL_Color bg = MazeContext_Get_BackgroundColor(as->maze_ctx);
  SDL_SetRenderDrawColor(as->renderer, bg.r, bg.g, bg.b, bg.a);
  SDL_RenderClear(as->renderer);

  GUI_Draw_Frame(as->maze_ui_state, as->maze_ctx);

  MazeContext_Update(as->maze_ctx, delta_time_ms);
  MazeContext_Render(as->renderer, as->maze_ctx);

  GUI_Render_Frame(as->renderer);

  SDL_RenderPresent(as->renderer);
  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (appstate != NULL) {
    AppState *as = (AppState *)appstate;
    SDL_DestroyRenderer(as->renderer);
    MazeContext_Destroy(as->maze_ctx);
    SDL_DestroyWindow(as->window);
    GUI_Deinit();
    SDL_free(appstate);
  }
}

#else

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

#endif
