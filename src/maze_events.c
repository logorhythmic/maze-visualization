#include "../include/maze_events.h"
#include <stdlib.h>

struct Event {
  EventType event_type;

  union {

    // This struct is accessed when EVENT_STATE_CHANGE
    struct {
      CellPos cell;
      CellState cell_state;
    } state_change;

    // This struct is accessed when EVENT_CELL_ACTION
    struct {
      CellPos cell1;
      CellPos cell2;
      CellAction cell_action;
    } cell_action;

  } data;
};

struct MazeEvents {
  Event *events;
  int current_event;
  int total_events;
  int capacity;
};

MazeEvents *MazeEvents_Create(int capacity) {
  MazeEvents *maze_events = calloc(1, sizeof(MazeEvents));
  Event *events = calloc(capacity, sizeof(Event));
  maze_events->events = events;
  maze_events->current_event = 0;
  maze_events->total_events = 0;
  maze_events->capacity = capacity;
  return maze_events;
}

bool MazeEvents_Expand(MazeEvents *old_events, int final_capacity) {
  Event *final_events =
      realloc(old_events->events, final_capacity * sizeof(Event));
  if (final_events == NULL) {
    return false;
  }
  old_events->events = final_events;
  old_events->capacity = final_capacity;
  return true;
}

bool MazeEvents_Add_StateChange(MazeEvents *maze_events, CellPos cell,
                                CellState cell_state) {
  if (maze_events->total_events >= maze_events->capacity) {
    return false;
  }

  Event new_event;
  new_event.event_type = EVENT_STATE_CHANGE;
  new_event.data.state_change.cell = cell;
  new_event.data.state_change.cell_state = cell_state;

  maze_events->events[maze_events->total_events] = new_event;
  maze_events->total_events += 1;
  return true;
}

bool MazeEvents_Add_CellAction(MazeEvents *maze_events, CellPos cell1,
                               CellPos cell2, CellAction cell_action) {

  if (maze_events->total_events >= maze_events->capacity) {
    return false;
  }

  Event new_event;
  new_event.event_type = EVENT_CELL_ACTION;
  new_event.data.cell_action.cell1 = cell1;
  new_event.data.cell_action.cell2 = cell2;
  new_event.data.cell_action.cell_action = cell_action;

  maze_events->events[maze_events->total_events] = new_event;
  maze_events->total_events += 1;
  return true;
}

bool MazeEvents_Step(MazeEvents *maze_events, Maze *maze) {

  if (maze_events->current_event >= maze_events->total_events) {
    maze_events->current_event = 0;
    return false;
  }

  Event curr_event = maze_events->events[maze_events->current_event++];
  EventType event_type = curr_event.event_type;

  switch (event_type) {

  case EVENT_STATE_CHANGE: {
    CellPos cell = curr_event.data.state_change.cell;
    CellState state = curr_event.data.state_change.cell_state;
    Maze_Set_CellState(maze, cell, state);
    break;
  }

  case EVENT_CELL_ACTION: {
    CellPos cell1 = curr_event.data.cell_action.cell1;
    CellPos cell2 = curr_event.data.cell_action.cell2;
    CellAction cell_action = curr_event.data.cell_action.cell_action;

    switch (cell_action) {
    case BREAK_WALL:
      Maze_Break_Wall(maze, cell1, cell2);
      break;

    case MOVE_HEAD:
      Maze_Set_CellState(maze, cell1, UNSET_STATE(STATE_LEAD_HEAD));
      Maze_Set_CellState(maze, cell2, STATE_LEAD_HEAD);
      break;

    case BREAK_WALL_AND_MOVE_HEAD:
      Maze_Break_Wall(maze, cell1, cell2);
      Maze_Set_CellState(maze, cell1, UNSET_STATE(STATE_LEAD_HEAD));
      Maze_Set_CellState(maze, cell2, STATE_LEAD_HEAD);
    }
  }
  }

  return true;
}

void MazeEvents_StepAll(MazeEvents *maze_events, Maze *maze) {
  while (MazeEvents_Step(maze_events, maze))
    ;
}

void Maze_Reset_EventNumber(MazeEvents *maze_events) {
  maze_events->current_event = 0;
}

void MazeEvents_Destroy(MazeEvents *maze_events) {
  if (maze_events == NULL) {
    return;
  }
  free(maze_events->events);
  free(maze_events);
  maze_events = NULL;
}
