#include "../include/maze_render.h"
#include "../include/colors.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <stdlib.h>

struct MazeRender {
  SDL_Renderer *renderer;
  SDL_Color wall_color;
  int wall_thickness;
  int cell_size;
  int start_pos_x;
  int start_pos_y;
};

typedef union {
  struct {
    Vector2 start;
    Vector2 end;
  };
  int points[4]; // [start.x, start.y, end.x, end.y]
} Line;

static inline void DrawLineThick(SDL_Renderer *r, Line line, SDL_Color col,
                                 int thickness) {
  SDL_SetRenderDrawColor(r, col.r, col.g, col.b, col.a);
  Vector2 start_pos = line.start;
  Vector2 end_pos = line.end;

  if (end_pos.y != start_pos.y) {
    SDL_FRect rect1 = {(float)start_pos.x, (float)start_pos.y, (float)thickness,
                       (float)(end_pos.y - start_pos.y + thickness)};
    SDL_RenderFillRect(r, &rect1);
    return;
  }

  if (end_pos.x != start_pos.x) {
    SDL_FRect rect2 = {(float)start_pos.x, (float)start_pos.y,
                       (float)(end_pos.x - start_pos.x + thickness),
                       (float)(thickness)};
    SDL_RenderFillRect(r, &rect2);
    return;
  }
}

MazeRender *Maze_Render_Create(SDL_Renderer *renderer, int cell_size,
                               Vector2 start_pos, int wall_thickness,
                               SDL_Color wall_color) {
  MazeRender *maze_render = calloc(1, sizeof(MazeRender));
  maze_render->renderer = renderer;
  maze_render->wall_color = wall_color;
  maze_render->wall_thickness = wall_thickness;
  maze_render->cell_size = cell_size;
  maze_render->start_pos_x = start_pos.x;
  maze_render->start_pos_y = start_pos.y;
  return maze_render;
}

static inline void DrawAndFill_Cell(CellPos cell_pos, Vector2 current_pos,
                                    MazeRender *maze_render, Maze *maze) {

  Cell curr_cell = maze->grid[Maze_Get_CellIndex(cell_pos, maze)];
  SDL_Renderer *r = maze_render->renderer;
  int curr_x = current_pos.x;
  int curr_y = current_pos.y;
  int cell_size = maze_render->cell_size;
  int wall_thick = maze_render->wall_thickness;
  Line line;

  SDL_FRect rect = {curr_x + wall_thick, curr_y + wall_thick, cell_size,
                    cell_size};
  // Below, the code draws the wall, if the wall does not exist, the cell rect
  // is modified.

  // Drawing North Wall.

  SDL_Color fill_col = COLOR_WHITE;

  switch (curr_cell.cell_state) {
  case STATE_BLANK:
    fill_col = COL_STATE_BLANK;
    break;

  case STATE_BACKTRACKED:
    fill_col = COL_STATE_BACKTRACKED;
    break;

  case STATE_SOLUTION:
    fill_col = COL_STATE_SOLUTION;
    break;

  case STATE_VISITED:
    fill_col = COL_STATE_VISITED;
    break;
  }

  SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
  SDL_RenderFillRect(r, &rect);

  if (!curr_cell.path_north) {

    line = (Line){.start = {curr_x, curr_y},

                  .end = {curr_x + cell_size, curr_y}};

    DrawLineThick(r, line, maze_render->wall_color, wall_thick);
  }
  // Drawing South Wall
  if (!curr_cell.path_south) {

    line = (Line){.start = {curr_x, curr_y + cell_size},

                  .end = {curr_x + cell_size, curr_y + cell_size}};

    DrawLineThick(r, line, maze_render->wall_color, wall_thick);
  }
  // Drawing east wall
  if (!curr_cell.path_east) {
    line = (Line){.start = {.x = curr_x + cell_size, .y = curr_y},

                  .end = {.x = curr_x + cell_size, curr_y + cell_size}};

    DrawLineThick(r, line, maze_render->wall_color, wall_thick);
  }
  // Drawing west wall
  if (!curr_cell.path_west) {
    line = (Line){.start = {.x = curr_x, .y = curr_y},

                  .end = {.x = curr_x, curr_y + cell_size}};

    DrawLineThick(r, line, maze_render->wall_color, wall_thick);
  }
}

void Maze_Render_Draw(MazeRender *maze_render, Maze *maze) {
  int start_x = maze_render->start_pos_x;
  int start_y = maze_render->start_pos_y;
  int curr_x = start_x;
  int curr_y = start_y;
  int cell_size = maze_render->cell_size;
  SDL_Renderer *r = maze_render->renderer;

  for (int row = 0; row < maze->rows; row++) {
    for (int col = 0; col < maze->columns; col++) {
      CellPos cell_pos = {row, col};
      Vector2 current_pos = {curr_x, curr_y};

      DrawAndFill_Cell(cell_pos, current_pos, maze_render, maze);
      curr_x += cell_size;
    }

    curr_x = start_x;
    curr_y += cell_size;
  }
}

void Maze_Render_Destroy(MazeRender *maze_render) { free(maze_render); }
