#pragma once
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_render.h>

#define DEFAULT_ROWS 30
#define DEFAULT_COLUMNS 40
#define DEFAULT_SPEED 2.0f

#define MAX_ROWS 100
#define MAX_COLUMNS 130

#define MIN_ROWS 5
#define MIN_COLUMNS 5

#ifdef __cplusplus

extern "C" {

#endif

typedef enum {
  MAZE_BLANK,
  MAZE_GENERATING,
  MAZE_GENERATED,
  MAZE_SOLVING,
  MAZE_SOLVED
} MazeMode;

typedef enum {
  GEN_DFS,
  GEN_PRIMS,
  GEN_KRUSKAL,
  TOTAL_GEN_ALGO,
} GenAlgo;

typedef enum {
  SOLVE_DFS,
  SOLVE_BFS,
  SOLVE_ASTAR,
  TOTAL_SOLVE_ALGO,
} SolveAlgo;

typedef struct {
  int rows;
  int columns;
  int start_pos[2];
  int end_pos[2];

  GenAlgo gen_algo;
  const char *gen_algo_names[TOTAL_GEN_ALGO];

  SolveAlgo solve_algo;
  const char *solve_algo_names[TOTAL_SOLVE_ALGO];

  bool animate;
  bool dark_mode;
  MazeMode maze_mode;

  float speed;
  float time_delay;
} MazeUIState;

typedef struct MazeContext MazeContext;

MazeUIState *MazeUIState_Create();

MazeContext *MazeContext_Create(MazeUIState *ui, SDL_Renderer *r);

void MazeContext_Event_SetMazeEndpoints(MazeContext *ctx);

void MazeContext_Event_SetMazeDimensions(MazeContext *ctx);

void MazeContext_Event_ResetMaze(MazeContext *ctx);

void MazeContext_Event_ClearSolution(MazeContext *ctx);

void MazeContext_Event_SkipAnimation(MazeContext *ctx);

void MazeContext_Event_GenerateMaze(MazeContext *ctx);

void MazeContext_Event_ChangeTheme(MazeContext *ctx);

void MazeContext_Event_SolveMaze(MazeContext *ctx);

SDL_Color MazeContext_Get_BackgroundColor(MazeContext *ctx);

void MazeContext_Render(SDL_Renderer *r, MazeContext *ctx);

void MazeContext_Update(MazeContext *ctx, uint64_t delta_time_ms);

/* Frees MazeContext
 * Also frees MazeUIState
 */
void MazeContext_Destroy(MazeContext *ctx);

#ifdef __cplusplus
}
#endif
