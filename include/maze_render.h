#pragma once
#include "maze.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

typedef struct MazeRender MazeRender;

typedef struct {
  int x;
  int y;
} Vector2;

MazeRender *Maze_Render_Create(SDL_Renderer *renderer, Vector2 view_port,
                               Vector2 view_padding, int wall_thickness,
                               SDL_Color wall_color, float soln_line_area,
                               SDL_Color generated_bg_color);

void Maze_Render(MazeRender *maze_render, Maze *maze);

void Maze_Render_Destroy(MazeRender *maze_render);
