#include "../include/generation.h"
#include <SDL3/SDL_stdinc.h>
#include <stdio.h>
#include <stdlib.h>

bool Get_ValidNeighbour(CellPos curr_pos, CellPos *neighbour_pos,
                        bool *visited_cells, Maze *maze) {
  CellPos north = {curr_pos.row - 1, curr_pos.col};
  CellPos south = {curr_pos.row + 1, curr_pos.col};
  CellPos east = {curr_pos.row, curr_pos.col + 1};
  CellPos west = {curr_pos.row, curr_pos.col - 1};
  int valid_count = 0;
  CellPos valid_cells[4];

  if (Maze_Is_CellValid(north, maze) &&
      !visited_cells[Maze_Get_CellIndex(north, maze)]) {
    valid_cells[valid_count] = north;
    valid_count += 1;
  }

  if (Maze_Is_CellValid(south, maze) &&
      !visited_cells[Maze_Get_CellIndex(south, maze)]) {
    valid_cells[valid_count] = south;
    valid_count += 1;
  }

  if (Maze_Is_CellValid(east, maze) &&
      !visited_cells[Maze_Get_CellIndex(east, maze)]) {
    valid_cells[valid_count] = east;
    valid_count += 1;
  }

  if (Maze_Is_CellValid(west, maze) &&
      !visited_cells[Maze_Get_CellIndex(west, maze)]) {
    valid_cells[valid_count] = west;
    valid_count += 1;
  }
  if (valid_count == 0) {
    return false;
  }
  *neighbour_pos = valid_cells[SDL_rand(valid_count)];

  return true;
}

MazeEvents *DFSGen_Generate_MazeEvents(Maze *maze) {
  int total_events = maze->total_cells * 10;
  int rows = maze->rows;
  int columns = maze->columns;
  MazeEvents *events = MazeEvents_Create(total_events);

  CellPos start_pos = maze->start_cell;

  CellPos *current_path = calloc(maze->total_cells, sizeof(CellPos));
  int top = 0;
  bool *visited_cells = calloc(maze->total_cells, sizeof(bool));
  current_path[top] = start_pos;
  visited_cells[Maze_Get_CellIndex(start_pos, maze)] = true;

  while (top >= 0) {
    CellPos neighbour;
    CellPos curr_cell = current_path[top];
    // setting current cell to lead head
    // MazeEvents_Add_StateChange(events, curr_cell, STATE_LEAD_HEAD);

    if (Get_ValidNeighbour(curr_cell, &neighbour, visited_cells, maze)) {
      // This logic is for advancing

      // 1. Adding neighbour to current_path
      top += 1;
      current_path[top] = neighbour;

      // 2. Adding valid neighbour to visited_cells
      int index = Maze_Get_CellIndex(neighbour, maze);
      visited_cells[index] = true;

      // 3. Changing state of cell
      CellState cell_state = STATE_GEN_VISITED;

      // 4. Breaking wall between current cell and neighbour cell
      CellPos cell1 = curr_cell;
      CellPos cell2 = neighbour;
      CellAction cell_action = BREAK_WALL;

      // 6. Adding State Generation Visited event to MazeEvents
      if (!MazeEvents_Add_StateChange(events, cell1, cell_state)) {
        printf("Somehow MazeEvents full. Wtf\n");
      }

      // 5. Adding BREAK_WALL Cell Action event to MazeEvents
      if (!MazeEvents_Add_CellAction(events, cell1, cell2, cell_action)) {
        printf("Somehow MazeEvents full. Wtf\n");
      }

    }

    else {
      // This logic is for backtracking

      // 1. Popping the element from the stack

      CellPos cell1 = current_path[top];
      top -= 1;

      // 2. Changing Cell state
      CellState cell_state = STATE_BACKTRACKED;

      // 3. Adding to MazeEvents
      MazeEvents_Add_StateChange(events, cell1, cell_state);
    }
  }

  Maze_SetAll_CellState(maze, STATE_GENERATED);

  printf("DFS SOLVE EVENTS GENERATED\n");
  free(current_path);
  free(visited_cells);
  return events;
}
