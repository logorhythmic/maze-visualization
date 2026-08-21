#pragma once
#include "maze.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>

typedef struct MazeRender MazeRender;

typedef struct {
  int x;
  int y;
} Vector2;

MazeRender *Maze_Render_Create(Vector2 view_port, Vector2 view_padding,
                               int wall_thickness, float soln_line_area);

void Maze_Render_ChangeTheme(MazeRender *maze_render, bool enable_dark_mode);

SDL_Color Maze_Render_GetBGColor(MazeRender *maze_render);

void Maze_Render_Change_WallThickness(MazeRender *maze_render,
                                      float wall_thickness);

void Maze_Render(SDL_Renderer *r, MazeRender *maze_render, Maze *maze);

void Maze_Render_Destroy(MazeRender *maze_render);
