#include "../include/maze_render.h"
#include "../include/colors.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
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

static bool can_connect(CellState current_state, CellState neighbour_state) {
  return (neighbour_state == STATE_SOLVE_VISITED ||
          neighbour_state == STATE_SOLUTION);
}

static void render_connection_rect(CellState current_state,
                                   CellState neighbour_state,
                                   SDL_Color fill_col, SDL_FRect *rect,
                                   SDL_Renderer *r) {

  SDL_Color temp_fill;
  if (current_state == neighbour_state) {
    temp_fill = fill_col;
    SDL_SetRenderDrawColor(r, temp_fill.r, temp_fill.g, temp_fill.b,
                           temp_fill.a);
    SDL_RenderFillRect(r, rect);
    return;
  }

  if (current_state == STATE_SOLUTION &&
      neighbour_state == STATE_SOLVE_VISITED) {
    temp_fill = COL_STATE_SOLVE_VISITED;
    SDL_SetRenderDrawColor(r, temp_fill.r, temp_fill.g, temp_fill.b,
                           temp_fill.a);
    SDL_RenderFillRect(r, rect);
    return;
  }

  if (current_state == STATE_SOLVE_VISITED &&
      neighbour_state == STATE_SOLUTION) {
    temp_fill = COL_STATE_SOLVE_VISITED;
    SDL_SetRenderDrawColor(r, temp_fill.r, temp_fill.g, temp_fill.b,
                           temp_fill.a);
    SDL_RenderFillRect(r, rect);
    return;
  }
}

static void Render_Solution_Line(SDL_Renderer *r, CellPos curr_pos,
                                 Vector2 top_left, int cell_size, Maze *maze,
                                 int wall_thickness) {
  // Additive rendering
  //  We render a block in the middle of the cell
  //  Then depending on if the adjacent cells are solution cells, we add blocks
  //  to render

  Cell *cell = &maze->grid[Maze_Get_CellIndex(curr_pos, maze)];

  float actual_size = cell_size;

  float mid_rect_area = 0.70f;

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

  // Render Middle Cell
  SDL_Color fill_col = COLOR_RENDER_BACKGROUND;

  if (cell->cell_state == STATE_SOLUTION) {
    fill_col = COL_STATE_SOLUTION;
  }
  if (cell->cell_state == STATE_SOLVE_VISITED) {
    fill_col = COL_STATE_SOLVE_VISITED;
  }

  // Check if path exists. If yes, then render a connection rect to show the
  // path

  if (cell->path_north) {
    CellPos north = {curr_pos.row - 1, curr_pos.col};
    Cell *north_cell = &maze->grid[Maze_Get_CellIndex(north, maze)];
    render_connection_rect(cell->cell_state, north_cell->cell_state, fill_col,
                           &north_half_rect, r);
  }
  //
  if (cell->path_south) {
    CellPos south = {curr_pos.row + 1, curr_pos.col};
    Cell *south_cell = &maze->grid[Maze_Get_CellIndex(south, maze)];
    render_connection_rect(cell->cell_state, south_cell->cell_state, fill_col,
                           &south_half_rect, r);
  }
  //
  if (cell->path_east) {
    CellPos east = {curr_pos.row, curr_pos.col + 1};
    Cell *east_cell = &maze->grid[Maze_Get_CellIndex(east, maze)];
    render_connection_rect(cell->cell_state, east_cell->cell_state, fill_col,
                           &east_half_rect, r);
  }

  if (cell->path_west) {
    CellPos west = {curr_pos.row, curr_pos.col - 1};
    Cell *west_cell = &maze->grid[Maze_Get_CellIndex(west, maze)];
    render_connection_rect(cell->cell_state, west_cell->cell_state, fill_col,
                           &west_half_rect, r);
  }

  SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
  SDL_RenderFillRect(r, &centre_rect);
}

static void DrawAndFill_Cell(CellPos cell_pos, Vector2 current_pos,
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

  case STATE_SOLVE_VISITED:
    fill_col = COLOR_NONE;
    Render_Solution_Line(r, cell_pos, current_pos, cell_size, maze,
                         maze_render->wall_thickness);
    break;

  case STATE_GEN_VISITED:
    fill_col = COL_STATE_GEN_VISITED;
    break;

  case STATE_SOLUTION:
    fill_col = COLOR_NONE;

    // Different logic is used to set solution
    Render_Solution_Line(r, cell_pos, current_pos, cell_size, maze,
                         maze_render->wall_thickness);
    break;

  case STATE_GENERATED:
    fill_col = COL_STATE_GENERATED;
    break;
  }

  if (fill_col.a > 0) {
    SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
    SDL_RenderFillRect(r, &rect);
  }

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

void Maze_Render(MazeRender *maze_render, Maze *maze) {
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

  SDL_Color start_col = COL_START_CELL;
  SDL_SetRenderDrawColor(r, start_col.r, start_col.g, start_col.b, start_col.a);
  SDL_RenderFillRect(r, &start_rect);
  SDL_Color end_col = COL_END_CELL;
  SDL_SetRenderDrawColor(r, end_col.r, end_col.g, end_col.b, end_col.a);
  SDL_RenderFillRect(r, &end_rect);
}

void Maze_Render_Destroy(MazeRender *maze_render) { free(maze_render); }
