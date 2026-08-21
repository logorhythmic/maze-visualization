#include "maze_render.h"
#include "colors.h"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

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
  // Get the actual scale SDL is applying (e.g., 1.25)
  float scale_x, scale_y;
  SDL_GetRenderScale(r, &scale_x, &scale_y);

  float sx = SDL_roundf(line.start.x * scale_x) / scale_x;
  float sy = SDL_roundf(line.start.y * scale_y) / scale_y;
  float ex = SDL_roundf(line.end.x * scale_x) / scale_x;
  float ey = SDL_roundf(line.end.y * scale_y) / scale_y;

  float snapped_thickness = SDL_roundf(thickness * scale_x) / scale_x;

  if (ey != sy) {
    SDL_FRect rect1 = {(float)sx, (float)sy, (float)thickness,
                       (float)(ey - sy + thickness)};
    SDL_RenderFillRect(r, &rect1);
    return;
  }

  if (ex != sx) {
    SDL_FRect rect2 = {(float)sx, (float)sy, (float)(ex - sx + thickness),
                       (float)(thickness)};
    SDL_RenderFillRect(r, &rect2);
    return;
  }
}

struct MazeRender {
  SDL_Color generated_bg_color;

  int cell_size;
  float soln_line_area;
  int wall_thickness;

  Vector2 view_dimensions;
  Vector2 view_padding;

  SDL_Color dark_colors[TOTAL_COLORS];
  SDL_Color light_colors[TOTAL_COLORS];
  const SDL_Color *curr_colors;
};

MazeRender *Maze_Render_Create(Vector2 view_dimensions, Vector2 view_padding,
                               int wall_thickness, float soln_line_area) {
  MazeRender *maze_render = calloc(1, sizeof(MazeRender));
  maze_render->wall_thickness = wall_thickness;
  maze_render->soln_line_area = soln_line_area;
  maze_render->view_dimensions = view_dimensions;
  maze_render->view_padding = view_padding;
  maze_render->cell_size = 0;
  maze_render->curr_colors = DEFAULT_DARK_COLORS;
  return maze_render;
}

void Maze_Render_Change_WallThickness(MazeRender *maze_render,
                                      float wall_thickness) {
  maze_render->wall_thickness = wall_thickness;
}

void Maze_Render_ChangeTheme(MazeRender *maze_render, bool enable_dark_mode) {
  if (enable_dark_mode) {
    maze_render->curr_colors = DEFAULT_DARK_COLORS;
  } else {
    maze_render->curr_colors = DEFAULT_LIGHT_COLORS;
  }
}

SDL_Color Maze_Render_GetBGColor(MazeRender *maze_render) {
  return maze_render->curr_colors[COL_RENDER_BACKGROUND];
}

static void draw_cell_connect(SDL_Renderer *renderer, CellPos curr_pos,
                              CellState connect_state, Vector2 top_left,
                              SDL_Color fill_col, MazeRender *maze_render,
                              Maze *maze) {
  // Additive rendering
  //  We render a block in the middle of the cell
  //  Then depending on if the adjacent cells are solution cells, we add blocks
  //  to render

  SDL_Renderer *r = renderer;
  Cell *cell = &maze->grid[Maze_Get_CellIndex(curr_pos, maze)];
  float cell_size = maze_render->cell_size;
  float wall_thickness = maze_render->wall_thickness;

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

static void draw_cell_fill_full(SDL_Renderer *r, Vector2 top_left,
                                SDL_Color fill_col, MazeRender *maze_render) {

  float cell_size = maze_render->cell_size;
  float wall_thickness = maze_render->wall_thickness;
  float inner_start_x = (float)top_left.x + wall_thickness;
  float inner_start_y = (float)top_left.y + wall_thickness;

  SDL_FRect rect = {inner_start_x, inner_start_y, cell_size, cell_size};
  SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
  SDL_RenderFillRect(r, &rect);
}

static void draw_cell_fill_offset(SDL_Renderer *r, Vector2 top_left,
                                  SDL_Color fill_col, MazeRender *maze_render,
                                  float offset) {

  float cell_size = maze_render->cell_size;
  float wall_thickness = maze_render->wall_thickness;
  float inner_start_x = (float)top_left.x + wall_thickness;
  float inner_start_y = (float)top_left.y + wall_thickness;

  SDL_FRect rect = {inner_start_x, inner_start_y, cell_size + offset,
                    cell_size + offset};
  SDL_SetRenderDrawColor(r, fill_col.r, fill_col.g, fill_col.b, fill_col.a);
  SDL_RenderFillRect(r, &rect);
}

static void Render_Cell_Wall(SDL_Renderer *r, CellPos cell_pos,
                             Vector2 current_pos, Maze *maze,
                             MazeRender *maze_render) {

  Cell curr_cell = maze->grid[Maze_Get_CellIndex(cell_pos, maze)];
  int curr_x = current_pos.x;
  int curr_y = current_pos.y;
  int cell_size = maze_render->cell_size;
  int wall_thick = maze_render->wall_thickness;
  SDL_Color wall_color = maze_render->curr_colors[COL_WALL];
  Line line;

  if (!curr_cell.path_north) {

    line = (Line){.start = {curr_x, curr_y},

                  .end = {curr_x + cell_size, curr_y}};

    DrawLineThick(r, line, wall_color, wall_thick);
  }
  // Drawing South Wall
  if (!curr_cell.path_south) {

    line = (Line){.start = {curr_x, curr_y + cell_size},

                  .end = {curr_x + cell_size, curr_y + cell_size}};

    DrawLineThick(r, line, wall_color, wall_thick);
  }
  // Drawing east wall
  if (!curr_cell.path_east) {
    line = (Line){.start = {.x = curr_x + cell_size, .y = curr_y},

                  .end = {.x = curr_x + cell_size, curr_y + cell_size}};

    DrawLineThick(r, line, wall_color, wall_thick);
  }
  // Drawing west wall
  if (!curr_cell.path_west) {
    line = (Line){.start = {.x = curr_x, .y = curr_y},

                  .end = {.x = curr_x, curr_y + cell_size}};

    DrawLineThick(r, line, wall_color, wall_thick);
  }
}
//

static void Render_Cell_Interior(SDL_Renderer *r, CellPos cell_pos,
                                 Vector2 top_left, Maze *maze,
                                 MazeRender *maze_render) {

  Cell curr_cell = maze->grid[Maze_Get_CellIndex(cell_pos, maze)];

  uint8_t state = curr_cell.cell_state;

  switch (state & BASE_STATE_MASK) {

  case STATE_BLANK:

    if (state & STATE_FRONTIER) {
      draw_cell_fill_full(r, top_left,
                          maze_render->curr_colors[COL_STATE_FRONTIER],
                          maze_render);

    } else if (state & STATE_BACKTRACKED) {
      draw_cell_fill_full(r, top_left,
                          maze_render->curr_colors[COL_STATE_BACKTRACKED],
                          maze_render);

    } else if (state & STATE_GEN_VISITED) {
      draw_cell_fill_full(r, top_left,
                          maze_render->curr_colors[COL_STATE_GEN_VISITED],
                          maze_render);

    } else if (state == STATE_BLANK) {
      draw_cell_fill_full(
          r, top_left, maze_render->curr_colors[COL_STATE_BLANK], maze_render);
    }
    break;

  case STATE_GENERATED:
    draw_cell_fill_full(r, top_left,
                        maze_render->curr_colors[COL_STATE_GENERATED],
                        maze_render);

    if (state & STATE_SOLVE_EXPLORED) {
      draw_cell_connect(
          r, cell_pos, (STATE_SOLVE_EXPLORED | STATE_FRONTIER), top_left,
          maze_render->curr_colors[COL_STATE_SOLVE_VISITED], maze_render, maze);
    }

    if (state & STATE_SOLUTION) {
      draw_cell_connect(r, cell_pos, STATE_SOLUTION, top_left,
                        maze_render->curr_colors[COL_STATE_SOLUTION],
                        maze_render, maze);
    }

    if (state & STATE_FRONTIER) {
      // float offset = 2 * maze_render->wall_thickness;
      // Vector2 custom = {top_left.x + offset, top_left.y + offset};
      // draw_cell_fill_offset(custom, COL_STATE_FRONTIER, maze_render,
      // -offset);
      draw_cell_connect(r, cell_pos, STATE_SOLVE_EXPLORED, top_left,
                        maze_render->curr_colors[COL_STATE_FRONTIER],
                        maze_render, maze);
    }
  }
}
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

void Maze_Render(SDL_Renderer *r, MazeRender *maze_render, Maze *maze) {

  assert(maze->columns > 0 && maze->rows > 0);
  Vector2 view_dimensions = maze_render->view_dimensions;
  Vector2 padding = maze_render->view_padding;
  int cell_size = (int)MIN((view_dimensions.x - 2 * padding.x) / maze->columns,
                           (view_dimensions.y - 2 * padding.y) / maze->rows);
  int start_x = (int)((view_dimensions.x - cell_size * maze->columns) / 2.0f);
  int start_y = (int)((view_dimensions.y - cell_size * maze->rows) / 2.0f);

  maze_render->cell_size = cell_size;

  int curr_x = start_x;
  int curr_y = start_y;

  // Drawing the generated color rect behind the maze
  SDL_FRect full_maze_rect = {start_x, start_y, maze->columns * cell_size,
                              maze->rows * cell_size};
  SDL_Color bg_col = maze_render->curr_colors[COL_STATE_GENERATED];
  SDL_SetRenderDrawColor(r, bg_col.r, bg_col.g, bg_col.b, bg_col.a);
  SDL_RenderFillRect(r, &full_maze_rect);

  for (int row = 0; row < maze->rows; row++) {
    for (int col = 0; col < maze->columns; col++) {
      CellPos cell_pos = {row, col};
      Vector2 top_left = {curr_x, curr_y};

      Render_Cell_Interior(r, cell_pos, top_left, maze, maze_render);
      Render_Cell_Wall(r, cell_pos, top_left, maze, maze_render);

      curr_x += cell_size;
    }

    curr_x = start_x;
    curr_y += cell_size;
  }

  //---------Displaying the start and end_cells with specific color----------
  Vector2 start = {(maze->start_cell.col * cell_size) + start_x,
                   (maze->start_cell.row * cell_size) + start_y};
  Vector2 end = {(maze->end_cell.col * cell_size) + start_x,
                 (maze->end_cell.row * cell_size) + start_y};

  // Filling Start and end Cell with color
  draw_cell_fill_full(r, start, maze_render->curr_colors[COL_START_CELL],
                      maze_render);
  draw_cell_fill_full(r, end, maze_render->curr_colors[COL_END_CELL],
                      maze_render);

  // Rendering the start and end cell walls
  Render_Cell_Wall(r, maze->start_cell, start, maze, maze_render);
  Render_Cell_Wall(r, maze->end_cell, end, maze, maze_render);

  //-------------------------------------------------------------------------
}

void Maze_Render_Destroy(MazeRender *maze_render) { free(maze_render); }
