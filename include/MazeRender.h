#pragma once
#include "Maze.h"
#include <SDL3/SDL_render.h>

typedef struct MazeRender MazeRender;

typedef struct {
  int x;
  int y;
} Vector2;

MazeRender *MazeRender_Create(SDL_Renderer *renderer, int cell_size,
                              Vector2 maze_start_pos, int wall_thickness,
                              SDL_Color wall_color);

void MazeRender_Draw(MazeRender *maze_render, Maze *maze);

void MazeRender_Destroyr(MazeRender *maze_render);
