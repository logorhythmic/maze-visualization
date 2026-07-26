#pragma once
#include "maze.h"
#include "maze_events.h"

MazeEvents *DFSSolve_Generate_MazeEvents(Maze *maze);

MazeEvents *BFSSolve_Generate_MazeEvents(Maze *maze);
