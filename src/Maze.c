#include "../include/Maze.h"
#include <stdio.h>
#include <stdlib.h>

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

void Maze_Destroy(Maze *maze) {
  free(maze->grid);
  free(maze);
}
