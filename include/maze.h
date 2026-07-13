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

typedef enum {
  ACTION_NONE,
  BREAK_WALL,
} CellAction;

typedef struct {
  CellPos cell1;
  CellPos cell2;
  CellState cell_state;
  CellAction cell_action;
} Event;

typedef struct {
  Event *events;
  int current_event;
  int total_events;
  int capacity;
} MazeEvents;

static inline bool Maze_Is_CellValid(CellPos cell, Maze *maze) {
  if (cell.row < maze->rows && cell.row >= 0) {
    if (cell.col < maze->columns && cell.col >= 0) {
      return true;
    }
  }
  return false;
}

static inline int Maze_Get_CellIndex(CellPos cp, Maze *maze) {
  return cp.row * maze->columns + cp.col;
}

Maze *Maze_Create(int rows, int columns, CellPos start_cell, CellPos end_cell);

void Maze_Break_Wall(CellPos cell1, CellPos cell2, Maze *maze);

void Maze_Reset(Maze *maze);

void Maze_Destroy(Maze *maze);

//------------------------Maze Events------------------------------------
MazeEvents *Maze_Create_Events(int capacity);

void Maze_Destroy_Events(MazeEvents *maze_events);

bool Maze_Add_Event(MazeEvents *maze_events, CellPos cell1, CellPos cell2,
                    CellState cell_state, CellAction cell_action);

bool Maze_Step_Event(MazeEvents *maze_events, Maze *maze);

void Maze_Reset_EventNumber(MazeEvents *maze_events);

void Maze_StepAll_Event(MazeEvents *maze_events, Maze *maze);
//----------------------------------------------------------------------
