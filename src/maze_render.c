#include "../include/maze_render.h"
#include "../include/colors.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct MazeRender {
  SDL_Renderer *renderer;
  SDL_Color generated_bg_color;
  SDL_Color wall_color;
  float soln_line_area;
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

static void DrawLineThick(SDL_Renderer *r, Line line, SDL_Color col,
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
                               SDL_Color wall_color, float soln_line_area,
                               SDL_Color generated_bg_color) {
  MazeRender *maze_render = calloc(1, sizeof(MazeRender));
  maze_render->renderer = renderer;
  maze_render->wall_color = wall_color;
  maze_render->generated_bg_color = generated_bg_color;
  maze_render->wall_thickness = wall_thickness;
  maze_render->soln_line_area = soln_line_area;
  maze_render->cell_size = cell_size;
  maze_render->start_pos_x = start_pos.x;
  maze_render->start_pos_y = start_pos.y;
  return maze_render;
}

static void draw_cell_connect(CellPos curr_pos, CellState connect_state,
                              Vector2 top_left, SDL_Color fill_col,
                              MazeRender *maze_render, Maze *maze) {
  // Additive rendering
  //  We render a block in the middle of the cell
  //  Then depending on if the adjacent cells are solution cells, we add blocks
  //  to render

  Cell *cell = &maze->grid[Maze_Get_CellIndex(curr_pos, maze)];
  float cell_size = maze_render->cell_size;
  float wall_thickness = maze_render->wall_thickness;
  SDL_Renderer *r = maze_render->renderer;

  float actual_size = cell_size;

  float mid_rect_area = maze_render->soln_line_area;

  float inner_start_x = (float)top_left.x + wall_thickness;
  float inner_start_y = (float)top_left.y + wall_thickness;

  // Do 1.0f and not 2.0f because with respect to the inner_start values, the
  // actual area is only covered by the bottom and rightmost walls
  float usable_size = cell_size - (1.0f * wall_thickness);

  // Size of the rect rendered in the middle
  float mid_rect_size = usable_size * mid_rect_area;

  // Offset determines how much space between wall and center rect
  float offset = ((usable_size - mid_rect_size) / 2.0f);

  float final_x = inner_start_x + offset;
  float final_y = inner_start_y + offset;

  SDL_FRect centre_rect = {final_x, final_y, mid_rect_size, mid_rect_size};

  SDL_FRect north_half_rect = {final_x, final_y - offset, mid_rect_size,
                               offset + wall_thickness};

  SDL_FRect south_half_rect = {final_x, final_y + mid_rect_size, mid_rect_size,
                               offset + wall_thickness};
  SDL_FRect east_half_rect = {final_x + mid_rect_size, final_y,
                              offset + wall_thickness, mid_rect_size};

  SDL_FRect west_half_rect = {final_x - offset, final_y,
                              offset + wall_thickness, mid_rect_size};

  // Check if path exists. If yes, then render a connection rect to show the
  // path

  if (cell->path_north) {
    CellPos north = {curr_pos.row - 1, curr_pos.col};
    Cell *north_cell = &maze->grid[Maze_Get_CellIndex(north, maze)];
    if (north_cell->cell_state & connect_state) {

      SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
      SDL_RenderFillRect(r, &north_half_rect);
    }
  }
  //
  if (cell->path_south) {
    CellPos south = {curr_pos.row + 1, curr_pos.col};
    Cell *south_cell = &maze->grid[Maze_Get_CellIndex(south, maze)];
    if (south_cell->cell_state & connect_state) {

      SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
      SDL_RenderFillRect(r, &south_half_rect);
    }
  }
  //
  if (cell->path_east) {
    CellPos east = {curr_pos.row, curr_pos.col + 1};
    Cell *east_cell = &maze->grid[Maze_Get_CellIndex(east, maze)];

    if (east_cell->cell_state & connect_state) {
      SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
      SDL_RenderFillRect(r, &east_half_rect);
    }
  }

  if (cell->path_west) {
    CellPos west = {curr_pos.row, curr_pos.col - 1};
    Cell *west_cell = &maze->grid[Maze_Get_CellIndex(west, maze)];
    if (west_cell->cell_state & connect_state) {

      SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
      SDL_RenderFillRect(r, &west_half_rect);
    }
  }

  SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
  SDL_RenderFillRect(r, &centre_rect);
}

static void draw_cell_fill_full(Vector2 top_left, SDL_Color fill_col,
                                MazeRender *maze_render) {

  float cell_size = maze_render->cell_size;
  float wall_thickness = maze_render->wall_thickness;
  SDL_Renderer *r = maze_render->renderer;
  float offset = 0.0f;
  float inner_start_x = (float)top_left.x + offset;
  float inner_start_y = (float)top_left.y + offset;

  SDL_FRect rect = {inner_start_x + wall_thickness,
                    inner_start_y + wall_thickness, cell_size - wall_thickness,
                    cell_size - wall_thickness};
  SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
  SDL_RenderFillRect(r, &rect);
}

static void Render_Cell_Wall(CellPos cell_pos, Vector2 current_pos, Maze *maze,
                             MazeRender *maze_render) {

  Cell curr_cell = maze->grid[Maze_Get_CellIndex(cell_pos, maze)];
  SDL_Renderer *r = maze_render->renderer;
  int curr_x = current_pos.x;
  int curr_y = current_pos.y;
  int cell_size = maze_render->cell_size;
  int wall_thick = maze_render->wall_thickness;
  Line line;

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

static void Render_Cell_Interior(CellPos cell_pos, Vector2 top_left, Maze *maze,
                                 MazeRender *maze_render) {

  Cell curr_cell = maze->grid[Maze_Get_CellIndex(cell_pos, maze)];
  SDL_Renderer *r = maze_render->renderer;
  int cell_size = maze_render->cell_size;
  int wall_thick = maze_render->wall_thickness;

  // Below, the code draws the wall, if the wall does not exist, the cell rect
  // is modified.

  uint8_t state = curr_cell.cell_state;

  switch (state & BASE_STATE_MASK) {

  case STATE_BLANK:

    if (state & STATE_BACKTRACKED) {
      draw_cell_fill_full(top_left, COL_STATE_BACKTRACKED, maze_render);

    } else if (state & STATE_GEN_VISITED) {
      draw_cell_fill_full(top_left, COL_STATE_GEN_VISITED, maze_render);

    } else if (state == STATE_BLANK) {
      draw_cell_fill_full(top_left, COL_STATE_BLANK, maze_render);
    }
    break;

  case STATE_GENERATED:
    draw_cell_fill_full(top_left, COL_STATE_GENERATED, maze_render);

    if (state & STATE_SOLVE_VISITED) {
      draw_cell_connect(cell_pos, STATE_SOLVE_VISITED, top_left,
                        COL_STATE_SOLVE_VISITED, maze_render, maze);
    }

    if (state & STATE_SOLUTION) {
      draw_cell_connect(cell_pos, STATE_SOLUTION, top_left, COL_STATE_SOLUTION,
                        maze_render, maze);
    }

    if (state & STATE_LEAD_HEAD) {
      printf("STate lead head triggered\n");
      draw_cell_connect(cell_pos, STATE_LEAD_HEAD, top_left,
                        COL_STATE_LEAD_HEAD, maze_render, maze);
    }
  }
}

void Maze_Render(MazeRender *maze_render, Maze *maze) {
  int start_x = maze_render->start_pos_x;
  int start_y = maze_render->start_pos_y;
  int curr_x = start_x;
  int curr_y = start_y;
  int cell_size = maze_render->cell_size;
  SDL_Renderer *r = maze_render->renderer;

  // Drawing the generated color rect behind the maze
  SDL_FRect full_maze_rect = {start_x, start_y, maze->columns * cell_size,
                              maze->rows * cell_size};
  SDL_Color bg_col = maze_render->generated_bg_color;
  SDL_SetRenderDrawColor(r, bg_col.r, bg_col.g, bg_col.b, bg_col.a);
  SDL_RenderFillRect(r, &full_maze_rect);

  for (int row = 0; row < maze->rows; row++) {
    for (int col = 0; col < maze->columns; col++) {
      CellPos cell_pos = {row, col};
      Vector2 top_left = {curr_x, curr_y};

      Render_Cell_Interior(cell_pos, top_left, maze, maze_render);
      Render_Cell_Wall(cell_pos, top_left, maze, maze_render);

      curr_x += cell_size;
    }

    curr_x = start_x;
    curr_y += cell_size;
  }

  // Displaying the start and end_cells with specific color
  Vector2 start = {(maze->start_cell.col * cell_size) + start_x,
                   (maze->start_cell.row * cell_size) + start_y};
  Vector2 end = {(maze->end_cell.col * cell_size) + start_x,
                 (maze->end_cell.row * cell_size) + start_y};
  float wall_thick = maze_render->wall_thickness;

  SDL_FRect start_rect = {start.x + wall_thick, start.y + wall_thick,
                          cell_size - wall_thick, cell_size - wall_thick};
  SDL_FRect end_rect = {end.x + wall_thick, end.y + wall_thick,
                        cell_size - wall_thick, cell_size - wall_thick};

  draw_cell_fill_full(start, COL_START_CELL, maze_render);
  draw_cell_fill_full(end, COL_END_CELL, maze_render);
}

void Maze_Render_Destroy(MazeRender *maze_render) { free(maze_render); }
