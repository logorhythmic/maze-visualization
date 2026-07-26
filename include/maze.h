#pragma once
#include <stdbool.h>

typedef enum {
  STATE_BLANK,         // When the Maze is a grid
  STATE_GENERATED,     // When Maze has been generated
  STATE_GEN_VISITED,   // When Cell has been visited by gen algo
  STATE_SOLVE_VISITED, // When Cell has been visited by solving algo
  STATE_BACKTRACKED,
  STATE_SOLUTION,
} CellState;

typedef struct CellPos CellPos;
typedef struct Cell Cell;
typedef struct Maze Maze;

struct CellPos {
  int row;
  int col;
};

struct Cell {
  CellPos cell_pos;
  CellState cell_state;
  bool path_north;
  bool path_south;
  bool path_east;
  bool path_west;
};

struct Maze {
  CellPos start_cell;
  CellPos end_cell;
  Cell *grid;
  int rows;
  int columns;
  int total_cells;
};

static inline bool Maze_Is_CellValid(CellPos cell, Maze *maze) {
  return (cell.row >= 0 && cell.row < maze->rows && cell.col >= 0 &&
          cell.col < maze->columns);
}

static inline bool Maze_Is_SameCell(CellPos a, CellPos b) {
  return (a.row == b.row && a.col == b.col);
}

static inline int Maze_Get_CellIndex(CellPos cp, Maze *maze) {
  return cp.row * maze->columns + cp.col;
}

Maze *Maze_Create(int rows, int columns, CellPos start_cell, CellPos end_cell);

void Maze_Break_Wall(Maze *maze, CellPos cell1, CellPos cell2);

void Maze_Reset(Maze *maze);

void Maze_Set_CellState(Maze *maze, CellPos cell_pos, CellState cell_state);

void Maze_SetAll_CellState(Maze *maze, CellState cell_state);

void Maze_Destroy(Maze *maze);
