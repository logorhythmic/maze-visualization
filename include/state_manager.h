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

typedef struct {
  int rows;
  int columns;
  int start_pos[2];
  int end_pos[2];

  bool animate;
  bool skipRequest;
  MazeMode maze_mode;
  float speed;
  float time_delay;
} MazeUIState;

typedef struct MazeContext MazeContext;

MazeUIState *MazeUIState_Create();

MazeContext *MazeContext_Create(MazeUIState *ui, SDL_Renderer *r);

void MazeContext_Process_Event(MazeContext *ctx, const SDL_Event *event);

void MazeContext_Set_MazeDimensions(MazeContext *ctx);

void MazeContext_Set_MazeEndpoints(MazeContext *ctx);

void MazeContext_Render(SDL_Renderer *r, MazeContext *ctx);

void MazeContext_Update(MazeContext *ctx, uint64_t delta_time_ms);

/* Frees MazeContext
 * Also frees MazeUIState
 */
void MazeContext_Destroy(MazeContext *ctx);

#ifdef __cplusplus
}
#endif
