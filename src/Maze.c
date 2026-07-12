#include "../include/Maze.h"
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

void Maze_Break_Wall(CellPos cell1, CellPos cell2, Maze *maze) {
  if (!Maze_Is_CellValid(cell1, maze) || !(Maze_Is_CellValid(cell2, maze))) {
    return;
  }
  Cell *curr_cell = &maze->grid[get_cell_index(cell1, maze)];
  Cell *neighbour_cell = &maze->grid[get_cell_index(cell2, maze)];

  if (abs(cell1.row - cell2.row) == 1 && cell1.col == cell2.col) {
    printf("ROw if trigegred\n");
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
    printf("Column if trigegred\n");
    if (cell1.col < cell2.col) {
      printf("Column1 if trigegred\n");
      // Cell1 is West of Cell2
      curr_cell->path_east = true;
      neighbour_cell->path_west = true;
    } else {
      printf("Column2 if trigegred\n");
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
void Maze_Destroy_Events(MazeEvents *maze_events) {
  free(maze_events->events);
  free(maze_events);
}

bool Maze_Add_Event(MazeEvents *maze_events, CellPos cell1, CellPos cell2,
                    CellState cell_state, CellAction cell_action) {
  if (maze_events->total_events > maze_events->capacity) {
    printf("MazeEvents full\n");
    return false;
  }
  maze_events->events->cell1 = cell1;
  maze_events->events->cell2 = cell2;
  maze_events->events->cell_state = cell_state;
  maze_events->events->cell_action = cell_action;
  maze_events->total_events += 1;
  return true;
}

bool Maze_Step_Event(MazeEvents *maze_events, Maze *maze) {
  if (maze_events->current_event == maze_events->total_events - 1) {
    return false;
    maze_events->current_event = 0;
  }
  CellPos current_cell = maze_events->events->cell1;
  maze->grid[get_cell_index(current_cell, maze)].cell_state =
      maze_events->events->cell_state;

  switch (maze_events->events->cell_action) { case BREAK_WALL: }
  maze_events->current_event += 1;
  return true;
}

void Maze_Destroy(Maze *maze) {
  free(maze->grid);
  free(maze);
}
