#include "../include/maze.h"
#include <stdio.h>
#include <stdlib.h>

static inline int get_cell_index(CellPos cp, Maze *maze) {
  return cp.row * maze->columns + cp.col;
}

Maze *Maze_Create(int rows, int columns, CellPos start_cell, CellPos end_cell) {
  Maze *maze = calloc(1, sizeof(Maze));

  if (maze == NULL) {
    fprintf(stderr, "Error: Calloc failed for maze allocation");
    return NULL;
  }
  int total_cells = rows * columns;
  Cell *grid = calloc(total_cells, sizeof(Cell));

  if (grid == NULL) {
    fprintf(stderr, "Error: Calloc failed for grid allocation");
    return NULL;
  }
  maze->grid = grid;
  maze->start_cell = start_cell;
  maze->end_cell = end_cell;
  maze->rows = rows;
  maze->columns = columns;
  maze->total_cells = total_cells;
  return maze;
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
    curr_cell->cell_state = cell_state;
  }
}

void Maze_Break_Wall(CellPos cell1, CellPos cell2, Maze *maze) {
  if (!Maze_Is_CellValid(cell1, maze) || !(Maze_Is_CellValid(cell2, maze))) {
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

MazeEvents *Maze_Create_Events(int capacity) {
  MazeEvents *maze_events = calloc(1, sizeof(MazeEvents));
  Event *events = calloc(capacity, sizeof(Event));
  maze_events->events = events;
  maze_events->current_event = 0;
  maze_events->total_events = 0;
  maze_events->capacity = capacity;
  return maze_events;
}

bool Maze_Expand_Events(MazeEvents *old_events, int final_capacity) {
  Event *final_events =
      realloc(old_events->events, final_capacity * sizeof(Event));
  if (final_events == NULL) {
    return false;
  }
  old_events->events = final_events;
  old_events->capacity = final_capacity;
  return true;
}

void Maze_Destroy_Events(MazeEvents *maze_events) {
  if (maze_events == NULL) {
    return;
  }
  free(maze_events->events);
  free(maze_events);
}

bool Maze_Add_Event(MazeEvents *maze_events, CellPos cell1, CellPos cell2,
                    CellState cell_state, CellAction cell_action) {
  if (maze_events->total_events >= maze_events->capacity) {
    return false;
  }
  int curr_event = maze_events->total_events;
  maze_events->events[curr_event].cell1 = cell1;
  maze_events->events[curr_event].cell2 = cell2;
  maze_events->events[curr_event].cell_state = cell_state;
  maze_events->events[curr_event].cell_action = cell_action;
  maze_events->total_events += 1;
  return true;
}

bool Maze_Step_Event(MazeEvents *maze_events, Maze *maze) {

  if (maze_events->current_event >= maze_events->total_events) {
    maze_events->current_event = 0;
    return false;
  }
  int curr_event = maze_events->current_event;
  CellPos current_cell = maze_events->events[curr_event].cell1;
  maze->grid[get_cell_index(current_cell, maze)].cell_state =
      maze_events->events[curr_event].cell_state;

  switch (maze_events->events[curr_event].cell_action) {
  case ACTION_NONE:
    break;

  case BREAK_WALL:
    Maze_Break_Wall(maze_events->events[curr_event].cell1,
                    maze_events->events[curr_event].cell2, maze);
    break;
  }
  maze_events->current_event += 1;
  return true;
}

void Maze_StepAll_Event(MazeEvents *maze_events, Maze *maze) {
  while (Maze_Step_Event(maze_events, maze))
    ;
}

void Maze_Reset_EventNumber(MazeEvents *maze_events) {
  maze_events->current_event = 0;
}

void Maze_Destroy(Maze *maze) {
  free(maze->grid);
  free(maze);
}
