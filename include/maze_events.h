#pragma once

#include "maze.h"

typedef struct Event Event;

typedef struct MazeEvents MazeEvents;

typedef enum {
  BREAK_WALL,
  MOVE_HEAD, // Change lead head from cell 1 to cell 2
  BREAK_WALL_AND_MOVE_HEAD
} CellAction;

#define EVENT_ADVANCE_FLAG 0x80

MazeEvents *MazeEvents_Create(int capacity);

// Expanding the number of events that Maze Events can hold
bool MazeEvents_Expand(MazeEvents *old_events, int final_capacity);

bool MazeEvents_Add_StateChange(MazeEvents *maze_events, CellPos cell1,
                                CellState cell_state);

bool MazeEvents_Add_StateChangeAll(MazeEvents *maze_events,
                                   CellState cell_state);

/* Add event to perform one of one of the actions defined in CellAction enum
 */
bool MazeEvents_Add_CellAction(MazeEvents *maze_events, CellPos cell1,
                               CellPos cell2, CellAction cell_action);

/* Function call to begin batching events
 * Every State Change or Cell Action called between this function call
 * and MazeEvents_End_Batch() will be executed in a single tick.
 */
void MazeEvents_Begin_Batch(MazeEvents *maze_events);

/* Function call to end batching events
 * Every StateChange or CellAction called between this function call
 * and MazeEvents_End_Batch() will be executed in a single tick.
 */
void MazeEvents_End_Batch(MazeEvents *maze_events);

/* Steps through a single event OR
 * all events belonging to the same batch.
 */
bool MazeEvents_Step(MazeEvents *maze_events, Maze *maze);

/* Steps through all events at once and shows final result
 */
void MazeEvents_StepAll(MazeEvents *maze_events, Maze *maze);

/* Resets event index to 0
 */
void MazeEvents_Reset_Index(MazeEvents *maze_events);

void MazeEvents_Destroy(MazeEvents *maze_events);
