#pragma once

#include "maze.h"

typedef struct Event Event;

typedef struct MazeEvents MazeEvents;
typedef enum {
  BREAK_WALL,
} CellAction;

typedef enum {
  EVENT_STATE_CHANGE,
  EVENT_CELL_ACTION,
} EventType;

MazeEvents *MazeEvents_Create(int capacity);

// Expanding the number of events that Maze Events can hold
bool MazeEvents_Expand(MazeEvents *old_events, int final_capacity);

bool MazeEvents_Add_StateChange(MazeEvents *maze_events, CellPos cell1,
                                CellState cell_state);

bool MazeEvents_Add_CellAction(MazeEvents *maze_events, CellPos cell1,
                               CellPos cell2, CellAction cell_action);

bool MazeEvents_Step(MazeEvents *maze_events, Maze *maze);

void MazeEvents_StepAll(MazeEvents *maze_events, Maze *maze);

void MazeEvents_Reset_Index(MazeEvents *maze_events);

void MazeEvents_Destroy(MazeEvents *maze_events);
