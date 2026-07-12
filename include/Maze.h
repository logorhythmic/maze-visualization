#pragma once
#include <stdbool.h>

typedef struct {
  int row;
  int col;
} CellPos;

typedef enum {
  STATE_BLANK,
  STATE_VISITED,
  STATE_BACKTRACKED,
  STATE_SOLUTION,
} CellState;

typedef struct {
  CellPos cell_pos;
  CellState cell_state;
  bool path_north;
  bool path_south;
  bool path_east;
  bool path_west;
} Cell;

typedef struct {
  CellPos start_cell;
  CellPos end_cell;
  Cell *grid;
  int rows;
  int columns;
  int total_cells;
} Maze;

Maze *Maze_Create(int rows, int columns, CellPos start_cell, CellPos end_cell);
void Maze_Destroy(Maze *maze);
