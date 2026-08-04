#pragma once
#include "maze.h"
#include "maze_events.h"

/* Maze Generation using Randomized DFS
 */
MazeEvents *DFSGen_Generate_MazeEvents(Maze *maze);

/* Maze Generation using Randomized Prims
 */
MazeEvents *PrimsGen_Generate_MazeEvents(Maze *maze);
