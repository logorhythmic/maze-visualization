#include "include/Maze.h"
#include "include/MazeRender.h"
#include "include/colors.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <stdio.h>

#define SCR_WIDTH 900
#define SCR_HEIGHT 900

#define CELL_SIZE 100
#define MAZE_START_POS ((Vector2){45, 45})
#define COLUMNS 8
#define ROWS 8
#define WALL_THICKNESS 3

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

  CellPos start_cell = {0, 0};
  CellPos end_cell = {9, 9};
  Maze *maze = Maze_Create(ROWS, COLUMNS, start_cell, end_cell);
  MazeRender *maze_render = MazeRender_Create(
      renderer, CELL_SIZE, MAZE_START_POS, WALL_THICKNESS, COLOR_BLACK);

  bool done = false;
  while (!done) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        done = true;
      }
    }

    SDL_Color bg = COLOR_RENDER_BACKGROUND;
    SDL_RenderClear(renderer);
    MazeRender_Draw(maze_render, maze);
    SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderPresent(renderer);
  }

  Maze_Destroy(maze);
  MazeRender_Destroyr(maze_render);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  return 0;
}
