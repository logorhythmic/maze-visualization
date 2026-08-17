#include "maze.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MAZE_ROW 150
#define MAX_MAZE_COL 150
#define MAX_MAZE_SIZE MAX_MAZE_ROW *MAX_MAZE_COL

static inline int get_cell_index(CellPos cp, Maze *maze) {
  return cp.row * maze->columns + cp.col;
}

Maze *Maze_Create() {
  Maze *maze = calloc(1, sizeof(Maze));
  Cell *grid = calloc(MAX_MAZE_SIZE, sizeof(Cell));
  maze->grid = grid;
  maze->rows = 0;
  maze->columns = 0;
  maze->total_cells = 0;
  return maze;
}

void Maze_Set_Dimensions(Maze *maze, int rows, int columns) {
  if (maze == NULL) {
    fprintf(stderr, "Error: Maze Pointer is NULL\n");
    return;
  }
  memset(maze->grid, 0, sizeof(Cell) * maze->total_cells);
  maze->rows = rows;
  maze->columns = columns;
  maze->total_cells = rows * columns;
  Maze_Reset(maze);
}

void Maze_Set_Endpoints(Maze *maze, CellPos start, CellPos end) {
  maze->start_cell = start;
  maze->end_cell = end;
}

void Maze_Reset(Maze *maze) {
  for (int i = 0; i < maze->total_cells; i++) {
    Cell *curr_cell = &maze->grid[i];
    curr_cell->path_north = false;
    curr_cell->path_south = false;
    curr_cell->path_east = false;
    curr_cell->path_west = false;
    curr_cell->cell_state = STATE_BLANK;
  }
}

void Maze_SetAll_CellState(Maze *maze, CellState cell_state) {

  for (int i = 0; i < maze->total_cells; i++) {

    Cell *curr_cell = &maze->grid[i];
    if (cell_state == STATE_GENERATED || cell_state == STATE_BLANK) {
      curr_cell->cell_state = cell_state;
      continue;
    }

    // Check if inversion needs to occur
    if (cell_state & STATE_CLEAR_MASK) {
      CellState state_to_remove = cell_state & ~STATE_CLEAR_MASK;
      curr_cell->cell_state &= ~state_to_remove;
    } else {
      curr_cell->cell_state |= cell_state;
    }
  }
}

uint8_t Maze_Get_CellState(Maze *maze, CellPos cell_pos) {
  return maze->grid[get_cell_index(cell_pos, maze)].cell_state;
}

void Maze_Set_CellState(Maze *maze, CellPos cell_pos, CellState cell_state) {
  Cell *curr_cell = &maze->grid[get_cell_index(cell_pos, maze)];

  if (cell_state == STATE_GENERATED || cell_state == STATE_BLANK) {
    curr_cell->cell_state = cell_state;
    return;
  }

  // Check if inversion needs to occur
  if (cell_state & STATE_CLEAR_MASK) {
    CellState state_to_remove = cell_state & ~STATE_CLEAR_MASK;
    curr_cell->cell_state &= ~state_to_remove;
  } else {
    curr_cell->cell_state |= cell_state;
  }
}

void Maze_Break_Wall(Maze *maze, CellPos cell1, CellPos cell2) {
  if (!Maze_Is_CellValid(cell1, maze) || !(Maze_Is_CellValid(cell2, maze))) {
    printf("Invalid CellPos to break walls\n");
    return;
  }

  if (Maze_Is_SameCell(cell1, cell2)) {
    printf("Cannot break walls between the same cell\n");
    return;
  }

  Cell *curr_cell = &maze->grid[get_cell_index(cell1, maze)];
  Cell *neighbour_cell = &maze->grid[get_cell_index(cell2, maze)];

  if (abs(cell1.row - cell2.row) == 1 && cell1.col == cell2.col) {
    if (cell1.row < cell2.row) {
      // Cell1 is North to Cell2
      curr_cell->path_south = true;
      neighbour_cell->path_north = true;
    } else {
      // Cell1 is South of Cell2
      curr_cell->path_north = true;
      neighbour_cell->path_south = true;
    }
  }

  else if (abs(cell1.col - cell2.col) == 1 && cell1.row == cell2.row) {
    if (cell1.col < cell2.col) {
      // Cell1 is West of Cell2
      curr_cell->path_east = true;
      neighbour_cell->path_west = true;
    } else {
      // Cell1 is East of Cell 2
      curr_cell->path_west = true;
      neighbour_cell->path_east = true;
    }
  }
}

void Maze_Destroy(Maze *maze) {
  free(maze->grid);
  free(maze);
}
