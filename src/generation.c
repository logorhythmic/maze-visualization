#include "generation.h"
#include <SDL3/SDL_stdinc.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int Get_ValidNeighbour(CellPos curr_pos, CellPos *neighbours,
                       bool *visited_cells, Maze *maze) {
  CellPos north = {curr_pos.row - 1, curr_pos.col};
  CellPos south = {curr_pos.row + 1, curr_pos.col};
  CellPos east = {curr_pos.row, curr_pos.col + 1};
  CellPos west = {curr_pos.row, curr_pos.col - 1};
  int valid_count = 0;
  CellPos valid_cells[4];

  if (Maze_Is_CellValid(north, maze) &&
      !visited_cells[Maze_Get_CellIndex(north, maze)]) {
    neighbours[valid_count] = north;
    valid_count += 1;
  }

  if (Maze_Is_CellValid(south, maze) &&
      !visited_cells[Maze_Get_CellIndex(south, maze)]) {
    neighbours[valid_count] = south;
    valid_count += 1;
  }

  if (Maze_Is_CellValid(east, maze) &&
      !visited_cells[Maze_Get_CellIndex(east, maze)]) {
    neighbours[valid_count] = east;
    valid_count += 1;
  }

  if (Maze_Is_CellValid(west, maze) &&
      !visited_cells[Maze_Get_CellIndex(west, maze)]) {
    neighbours[valid_count] = west;
    valid_count += 1;
  }

  return valid_count;
}

MazeEvents *DFSGen_Generate_MazeEvents(Maze *maze) {
  int total_events = maze->total_cells * 10;
  MazeEvents *events = MazeEvents_Create(total_events);

  CellPos start_pos = maze->start_cell;
  CellPos curr_head = start_pos;
  MazeEvents_Add_StateChange(events, curr_head, STATE_FRONTIER);

  // MazeEvents_Add_StateChange(events, previous_cell, STATE_FRONTIER);

  CellPos *current_path = calloc(maze->total_cells, sizeof(CellPos));
  int top = -1;
  bool *visited_cells = calloc(maze->total_cells, sizeof(bool));
  current_path[++top] = start_pos;
  visited_cells[Maze_Get_CellIndex(start_pos, maze)] = true;

  CellPos backtracked_cell;
  while (top >= 0) {
    CellPos curr_cell = current_path[top];

    CellPos neighbours[4];
    int total_valid_neighbours =
        Get_ValidNeighbour(curr_cell, neighbours, visited_cells, maze);

    if (total_valid_neighbours) {
      // This logic is for advancing

      CellPos random_neighbour = neighbours[SDL_rand(total_valid_neighbours)];
      // 1. Adding neighbour to current_path
      current_path[++top] = random_neighbour;

      // 2. Adding valid neighbour to visited_cells
      int index = Maze_Get_CellIndex(random_neighbour, maze);
      visited_cells[index] = true;

      MazeEvents_Add_StateChange(events, curr_cell, STATE_GEN_VISITED);

      MazeEvents_Begin_Batch(events);
      MazeEvents_Add_CellAction(events, curr_cell, random_neighbour,
                                BREAK_WALL);
      MazeEvents_Add_CellAction(events, curr_cell, random_neighbour, MOVE_HEAD);
      MazeEvents_End_Batch(events);

      // MazeEvents_Add_CellAction(events, curr_cell, random_neighbour,
      //                           BREAK_WALL_AND_MOVE_HEAD);

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

typedef struct {
  CellPos current;
  CellPos parent;
} Node;

MazeEvents *PrimsGen_Generate_MazeEvents(Maze *maze) {
  int total_events = maze->total_cells * 6;
  int total_nodes = maze->total_cells;
  int total_edges = -1;
  bool *visited_cells = calloc(maze->total_cells, sizeof(bool));

  Node *random_queue = calloc(maze->total_cells * 2, sizeof(Node));
  int rear = 0;
  // CellPos random_start_cell = {SDL_rand(maze->rows),
  // SDL_rand(maze->columns)};
  CellPos start_cell = maze->start_cell;
  random_queue[rear++] = (Node){start_cell, (CellPos){-1, -1}};

  MazeEvents *events = MazeEvents_Create(total_events);

  while (rear >= 0) {
    int random_index = SDL_rand(rear);
    Node curr_node = random_queue[random_index];

    // Replacing current element with the one from the end and reducing the size
    --rear;
    if (rear >= 0) {
      random_queue[random_index] = random_queue[rear];
    }

    if (!visited_cells[Maze_Get_CellIndex(curr_node.current, maze)]) {
      CellPos neighbours[4];
      int total_valid_neighbours = Get_ValidNeighbour(
          curr_node.current, neighbours, visited_cells, maze);

      MazeEvents_Begin_Batch(events);
      MazeEvents_Add_CellAction(events, curr_node.current, curr_node.parent,
                                BREAK_WALL);
      MazeEvents_Add_StateChange(events, curr_node.current, STATE_GENERATED);

      // Iterating through the neighbours and adding them to the queue
      for (int i = 0; i < total_valid_neighbours; i++) {
        random_queue[rear++] = (Node){neighbours[i], curr_node.current};
        MazeEvents_Add_StateChange(events, neighbours[i], STATE_FRONTIER);
      }
      MazeEvents_End_Batch(events);

      visited_cells[Maze_Get_CellIndex(curr_node.current, maze)] = true;
      total_edges += 1;
    }

    if (total_edges == total_nodes - 1) {
      break;
    }
  }

  Maze_SetAll_CellState(maze, STATE_GENERATED);

  free(visited_cells);
  free(random_queue);

  return events;
}
