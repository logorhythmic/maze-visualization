#include "../include/solving.h"
#include <stdio.h>
#include <stdlib.h>

bool Get_Unvisited_Neighbour(CellPos curr_pos, CellPos *neighbour_pos,
                             Maze *maze, const bool *visited) {
  Cell *cell = &maze->grid[Maze_Get_CellIndex(curr_pos, maze)];

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

  MazeEvents *events = MazeEvents_Create(maze->total_cells * 4);
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

      // Adding State change
      MazeEvents_Add_StateChange(events, current, STATE_SOLUTION);

      break;
    }

    // Advancing
    if (Get_Unvisited_Neighbour(current, &neighbour, maze, visited)) {

      // Adding Neighbour to current_path
      top += 1;
      current_path[top] = neighbour;

      // Adding neighbour to visited
      visited[Maze_Get_CellIndex(neighbour, maze)] = true;

      // Adding MazeEvent
      MazeEvents_Add_StateChange(events, current, STATE_SOLUTION);
    }

    // Backtracking
    else {
      // Popping Element from the stack
      top -= 1;
      MazeEvents_Add_StateChange(events, current, STATE_SOLUTION);
      MazeEvents_Add_StateChange(events, current, STATE_GENERATED);
    }
  }
  printf("Ending the while loop\n");
  free(current_path);
  free(visited);
  return events;
}

// Returns pointer to CellPos array of the final path.
// Size of array is equal to its depth.
// Populates final_length with final path lenght

CellPos *obtain_final_path(CellPos *came_from, int *final_length, Maze *maze) {
  // Obtaining the proper path and storing in the path array
  // Path length will be equal to the length
  CellPos start = maze->start_cell;
  CellPos parent = {-1, -1};

  // The last child is the end cell. Hence end_cell is assigned to child
  CellPos child = maze->end_cell;

  // Calculating the size of the final path
  CellPos curr = maze->end_cell;
  int path_length = 1;
  while (!Maze_Is_SameCell(curr, start)) {
    curr = came_from[Maze_Get_CellIndex(curr, maze)];
    path_length += 1;
  }
  path_length += 1; // To include the start cell too

  printf("Path Length is: %d\n", path_length);
  *final_length = path_length;

  CellPos *final_path = calloc(path_length, sizeof(CellPos));
  int count = 0;

  // Assigning the end cell to the final path
  final_path[count++] = maze->end_cell;

  while (!Maze_Is_SameCell(parent, start)) {
    parent = came_from[Maze_Get_CellIndex(child, maze)];

    final_path[count++] = parent;

    // The parent becomes the new child.
    // We find the parent of the parent and so on until we find the final
    // parent (the start cell)
    child = parent;
  }
  final_path[count] = start;
  printf("Size of count is equal to: %d\n", count);

  // Need to reverse the final_path. Currently it goes from end to start. It
  // needs to go from start to end
  CellPos *final_path_rev = calloc(path_length, sizeof(CellPos));
  int c = 0;

  // Reversing the final path
  for (int i = count; i >= 0; i--) {
    final_path_rev[c] = final_path[i];
    c++;
  }
  printf("Size of C is: %d\n", c);

  free(final_path);
  return final_path_rev;
}

MazeEvents *BFSSolve_Generate_MazeEvents(Maze *maze) {

  // Allocating the frontier array
  CellPos *frontier = calloc(maze->total_cells, sizeof(CellPos));
  int front = -1;
  int rear = -1; // Position to enqueue an element

  // To keep track of visited cells
  bool *visited = calloc(maze->total_cells, sizeof(bool));

  // To store the path history
  CellPos *came_from = calloc(maze->total_cells, sizeof(CellPos));

  MazeEvents *events = MazeEvents_Create(maze->total_cells * 3);

  frontier[++rear] = maze->start_cell;

  // Storing graph depth
  int depth = 0;

  while (front < rear) {
    CellPos current_pos = frontier[++front];
    CellPos neighbour_pos = {-1, -1};

    // If we find the target cell (end cell). We generate the final path and
    // then exit the loop
    if (Maze_Is_SameCell(current_pos, maze->end_cell)) {

      MazeEvents_Add_StateChange(events, current_pos, STATE_SOLVE_VISITED);

      // Unset all lead heads
      MazeEvents_Add_StateChangeAll(events, UNSET_STATE(STATE_FRONTIER));

      int path_length;
      CellPos *final_path = obtain_final_path(came_from, &path_length, maze);

      // Need to increase size of event array to add in the final path
      // visualization

      if (!MazeEvents_Expand(events, (maze->total_cells * 2) + path_length)) {
        printf(
            "Maze failed to expand events. Can not visualize the final path\n");
        return NULL;
      }

      for (int i = 0; i < path_length; i++) {
        MazeEvents_Add_StateChange(events, final_path[i], STATE_SOLUTION);
      }

      free(final_path);
      break;
    }

    int num_of_neighbours = 0;
    while (
        Get_Unvisited_Neighbour(current_pos, &neighbour_pos, maze, visited)) {

      // Adding neighbour_pos to visited array
      visited[Maze_Get_CellIndex(neighbour_pos, maze)] = true;

      // Enqueing the neighbour_pos
      frontier[++rear] = neighbour_pos;
      // MazeEvents_Add_StateChange(events, neighbour_pos, STATE_SOLVE_VISITED);

      // Adding cell to path history
      // Q: "Where did neighbour_cell come from?"->
      // A: "It came from current_cell"
      came_from[Maze_Get_CellIndex(neighbour_pos, maze)] = current_pos;
      num_of_neighbours++;
    }

    MazeEvents_Add_CellAction(events,
                              came_from[Maze_Get_CellIndex(current_pos, maze)],
                              current_pos, MOVE_HEAD);
    if (num_of_neighbours == 0) {
      MazeEvents_Add_StateChange(events, current_pos,
                                 UNSET_STATE(STATE_FRONTIER));
    }
    // Setting current_pos to STATE_SOLVE_VISITED
    MazeEvents_Add_StateChange(events, current_pos, STATE_SOLVE_VISITED);

    // Incrementing depth
    depth += 1;
  }

  printf("Ending the BFS solve while loop\n");
  printf("Depth of the path is: %d\n", depth);
  free(came_from);
  free(frontier);
  free(visited);
  return events;
}
