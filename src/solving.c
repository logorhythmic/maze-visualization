#include "../include/solving.h"
#include <stdio.h>
#include <stdlib.h>

bool Get_Unvisited_Neighbour(CellPos curr_pos, CellPos *neighbour_pos,
                             Maze *maze, const bool *visited) {
  Cell *cell = &maze->grid[Maze_Get_CellIndex(curr_pos, maze)];
  CellPos north = {curr_pos.row - 1, curr_pos.col};
  CellPos south = {curr_pos.row + 1, curr_pos.col};
  CellPos east = {curr_pos.row, curr_pos.col + 1};
  CellPos west = {curr_pos.row, curr_pos.col - 1};

  if (cell->path_north) {
    CellPos north = {curr_pos.row - 1, curr_pos.col};
    if (!visited[Maze_Get_CellIndex(north, maze)]) {
      *neighbour_pos = north;
      return true;
    }
  }

  if (cell->path_south) {
    CellPos south = {curr_pos.row + 1, curr_pos.col};
    if (!visited[Maze_Get_CellIndex(south, maze)]) {
      *neighbour_pos = south;
      return true;
    }
  }

  if (cell->path_east) {
    CellPos east = {curr_pos.row, curr_pos.col + 1};
    if (!visited[Maze_Get_CellIndex(east, maze)]) {
      *neighbour_pos = east;
      return true;
    }
  }

  if (cell->path_west) {
    CellPos west = {curr_pos.row, curr_pos.col - 1};
    if (!visited[Maze_Get_CellIndex(west, maze)]) {
      *neighbour_pos = west;
      return true;
    }
  }

  return false;
}

MazeEvents *DFSSolve_Generate_MazeEvents(Maze *maze) {

  // current_path is a STACK implementation
  CellPos *current_path = calloc(maze->total_cells, sizeof(CellPos));
  int top = 0;

  // To keep track of visited cells
  bool *visited = calloc(maze->total_cells, sizeof(bool));

  MazeEvents *events = Maze_Create_Events(maze->total_cells * 2);

  CellPos start = maze->start_cell;
  CellPos target = maze->end_cell;

  current_path[top] = start;
  visited[Maze_Get_CellIndex(start, maze)] = true;

  // Default value for invalid neighbour
  CellPos neighbour = {-1, -1};
  while (top >= 0) {
    CellPos current = current_path[top];

    // Checking if target has been reached
    if (current.row == target.row && current.col == target.col) {
      Maze_Add_Event(events, current, neighbour, STATE_SOLUTION, ACTION_NONE);
      break;
    }

    // Advancing
    if (Get_Unvisited_Neighbour(current, &neighbour, maze, visited)) {

      // Adding Neighbour to current_path
      top += 1;
      current_path[top] = neighbour;

      // Adding neighbour to visited
      visited[Maze_Get_CellIndex(neighbour, maze)] = true;

      // Adding a MazeEvent
      Maze_Add_Event(events, current, neighbour, STATE_SOLUTION, ACTION_NONE);
    }

    // Backtracking
    else {
      // Popping Element from the stack
      top -= 1;
      Maze_Add_Event(events, current, neighbour, STATE_BACKTRACKED,
                     ACTION_NONE);
    }
  }
  printf("Ending the while loop\n");
  free(current_path);
  free(visited);
  return events;
}
