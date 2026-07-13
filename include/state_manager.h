#pragma once
#include "maze.h"
#include "maze_render.h"

typedef struct SUBMODE_DFS_INFO SUBMODE_DFS_INFO;

typedef enum {
  IDLE,
  DISPLAY_MAZE_STATIC,
  DISPLAY_VISUALIZE,
} SUBMODE;

SUBMODE_DFS_INFO *DFSGen_Create_Submode(SDL_Renderer *renderer);
void DFS_Set_Submode(SUBMODE_DFS_INFO *sdi, SUBMODE new_state);
void DFS_Process_Submode(SDL_Renderer *r, SUBMODE_DFS_INFO *sdi,
                         uint64_t delta_time_ms);
void DFSGen_Destroy_Submode(SUBMODE_DFS_INFO *sdi);
