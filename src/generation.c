#include "../include/generation.h"
#include <SDL3/SDL_stdinc.h>
#include <assert.h>
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
  CellPos curr_head = start_pos;
  MazeEvents_Add_StateChange(events, curr_head, STATE_LEAD_HEAD);

  // MazeEvents_Add_StateChange(events, previous_cell, STATE_LEAD_HEAD);

  CellPos *current_path = calloc(maze->total_cells, sizeof(CellPos));
  int top = -1;
  bool *visited_cells = calloc(maze->total_cells, sizeof(bool));
  current_path[++top] = start_pos;
  visited_cells[Maze_Get_CellIndex(start_pos, maze)] = true;

  CellPos backtracked_cell;
  while (top >= 0) {
    CellPos neighbour;
    CellPos curr_cell = current_path[top];

    if (Get_ValidNeighbour(curr_cell, &neighbour, visited_cells, maze)) {
      // This logic is for advancing

      // 1. Adding neighbour to current_path
      current_path[++top] = neighbour;

      // 2. Adding valid neighbour to visited_cells
      int index = Maze_Get_CellIndex(neighbour, maze);
      visited_cells[index] = true;

      MazeEvents_Add_StateChange(events, curr_cell, STATE_GEN_VISITED);

      MazeEvents_Add_CellAction(events, curr_cell, neighbour,
                                BREAK_WALL_AND_MOVE_HEAD);

    }

    else {
      // This logic is for backtracking

      --top;
      MazeEvents_Add_StateChange(events, curr_cell, STATE_BACKTRACKED);
      if (top >= 0) {

        backtracked_cell = current_path[top];

        MazeEvents_Add_CellAction(events, curr_cell, backtracked_cell,
                                  MOVE_HEAD);
      }
    }
  }

  Maze_SetAll_CellState(maze, STATE_GENERATED);

  printf("DFS SOLVE EVENTS GENERATED\n");
  free(current_path);
  free(visited_cells);
  return events;
}
