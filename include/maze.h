#pragma once
#include <stdbool.h>
#include <stdint.h>

#define BASE_STATE_MASK 0x01

#define STATE_CLEAR_MASK ((uint8_t)0x80)

// Macro to convert a flag to its inverse
#define UNSET_STATE(state) (((CellState)(state) | STATE_CLEAR_MASK))

typedef enum {

  // First bit stores whether it is blank or generated
  // Both states are mutually exclusive
  STATE_BLANK = 0x00,     // When the Maze is a grid
  STATE_GENERATED = 0x01, // When Maze has been fully generated

  // Specific flags
  STATE_GEN_VISITED = 1 << 1,   // When Cell has been visited by gen algo
  STATE_SOLVE_VISITED = 1 << 2, // When Cell has been visited by solving algo
  STATE_FRONTIER = 1 << 3,
  STATE_BACKTRACKED = 1 << 4,
  STATE_SOLUTION = 1 << 5,
} CellState;

typedef struct CellPos CellPos;
typedef struct Cell Cell;
typedef struct Maze Maze;

struct CellPos {
  int row;
  int col;
};

struct Cell {
  uint8_t cell_state;
  CellPos cell_pos;
  bool path_north;
  bool path_south;
  bool path_east;
  bool path_west;
};

struct Maze {
  CellPos start_cell;
  CellPos end_cell;
  CellPos lead_head;
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
