#pragma once

#define COLOR_PINK ((SDL_Color){247, 141, 167, 255})
#define COLOR_PARCHMENT ((SDL_Color){236, 232, 222, 255}) // Hex: #ECE8DE
#define COLOR_WHITE ((SDL_Color){255, 255, 255, 255})
#define COLOR_OFF_WHITE ((SDL_Color){242, 240, 235, 255})
#define COLOR_CLOUD_SILVER ((SDL_Color){220, 220, 220, 255})
#define COLOR_LAPIS ((SDL_Color){0, 96, 255, 255})
#define COLOR_SILVER ((SDL_Color){180, 180, 180, 255})
#define COLOR_CHARCOAL ((SDL_Color){54, 69, 79, 255})
#define COLOR_BLACK ((SDL_Color){0, 0, 0, 255})
#define COLOR_NONE ((SDL_Color){255, 255, 255, 0})
#define COLOR_TRANSPERENT ((SDL_Color){0, 0, 0, 0})
#define COLOR_CARDINAL_RED ((SDL_Color){196, 30, 58, 255})
#define COLOR_CRIMSON_MIDNIGHT ((SDL_Color){100, 18, 32, 255})
#define COLOR_NEON_PINK ((SDL_Color){255, 92, 141, 255})
#define COLOR_MINT_GREEN ((SDL_Color){173, 235, 179, 255})
#define COLOR_DARK_AMETHYST ((SDL_Color){74, 35, 90, 255})
#define COLOR_LASER_VIOLET ((SDL_Color){187, 134, 252, 255})
#define COLOR_OCEAN_BLUE ((SDL_Color){12, 36, 97, 255})
#define COLOR_BURNT_MAROON ((SDL_Color){55, 12, 18, 255})
#define COLOR_EMERALD ((SDL_Color){0, 180, 0, 255})
#define COLOR_FOREST_GREEN ((SDL_Color){46, 111, 64, 255})
#define COLOR_MUTE_GOLD ((SDL_Color){235, 183, 24, 255})

#define COLOR_GOLD ((SDL_Color){239, 191, 4, 255})
#define COLOR_NEON_YELLOW ((SDL_Color){204, 255, 0, 255})
#define COLOR_AMBER_YELLOW ((SDL_Color){255, 191, 0, 255})
#define COLOR_DEEP_AMBER ((SDL_Color){204, 136, 0, 255})
#define COLOR_BURNT_OCHRE ((SDL_Color){178, 102, 0, 255})
#define COLOR_WARM_BRONZE ((SDL_Color){191, 114, 20, 255})

#define COLOR_EIGENGRAU ((SDL_Color){22, 22, 29, 255})
#define COLOR_CORAL_RED ((SDL_Color){255, 107, 107, 255})
#define COLOR_PLUM_PURPLE ((SDL_Color){142, 69, 133, 255})
#define COLOR_BRIGHT_CYAN ((SDL_Color){0, 230, 255, 255})
#define COLOR_DUSK_ROSE ((SDL_Color){192, 108, 132, 255})
#define COLOR_ALABASTER ((SDL_Color){248, 249, 250, 255})

#define COLOR_SLIGHT_WHITE ((SDL_Color){240, 240, 240, 240})

#include <SDL3/SDL_pixels.h>
typedef enum {
  COL_RENDER_BACKGROUND,

  COL_START_CELL,
  COL_END_CELL,

  COL_WALL,
  COL_STATE_BLANK,

  COL_STATE_GENERATED,
  COL_STATE_GEN_VISITED,

  COL_STATE_FRONTIER,
  COL_STATE_SOLUTION,
  COL_STATE_SOLVE_VISITED,

  COL_STATE_BACKTRACKED,
  TOTAL_COLORS,

} GridColors;

static const SDL_Color DEFAULT_LIGHT_COLORS[TOTAL_COLORS] = {

    [COL_RENDER_BACKGROUND] = COLOR_SLIGHT_WHITE,
    [COL_STATE_GENERATED] = COLOR_PARCHMENT,
    [COL_WALL] = COLOR_BLACK,
    [COL_START_CELL] = COLOR_CARDINAL_RED,
    [COL_END_CELL] = COLOR_FOREST_GREEN,
    [COL_STATE_BLANK] = COLOR_SILVER,
    [COL_STATE_FRONTIER] = COLOR_GOLD,
    [COL_STATE_GEN_VISITED] = COLOR_PINK,
    [COL_STATE_SOLVE_VISITED] = COLOR_PINK,
    [COL_STATE_BACKTRACKED] = COLOR_PARCHMENT,
    [COL_STATE_SOLUTION] = COLOR_LAPIS,
};

static const SDL_Color DEFAULT_DARK_COLORS[TOTAL_COLORS] = {

    [COL_RENDER_BACKGROUND] = COLOR_EIGENGRAU,
    [COL_STATE_GENERATED] = COLOR_BLACK,
    [COL_STATE_BACKTRACKED] = COLOR_BLACK,
    [COL_START_CELL] = COLOR_CORAL_RED,
    [COL_END_CELL] = COLOR_MINT_GREEN,
    [COL_STATE_FRONTIER] = COLOR_NEON_YELLOW,
    [COL_WALL] = COLOR_SILVER,
    [COL_STATE_BLANK] = COLOR_CHARCOAL,
    [COL_STATE_GEN_VISITED] = COLOR_BURNT_MAROON,
    [COL_STATE_SOLVE_VISITED] = COLOR_CRIMSON_MIDNIGHT,
    [COL_STATE_SOLUTION] = COLOR_BRIGHT_CYAN,
};

// --- Add these new colors to your definitions ---
#define COLOR_EIGENGRAU ((SDL_Color){22, 22, 29, 255})

// --- Light Theme remains mostly identical, as it was already well-balanced ---
// static const SDL_Color DEFAULT_LIGHT_COLORS[TOTAL_COLORS] = {
//     [COL_RENDER_BACKGROUND] = COLOR_WHITE,
//     [COL_STATE_GENERATED] = COLOR_PARCHMENT,
//     [COL_WALL] = COLOR_BLACK,
//     [COL_START_CELL] = COLOR_CARDINAL_RED,
//     [COL_END_CELL] = COLOR_FOREST_GREEN,
//     [COL_STATE_BLANK] = COLOR_SILVER,
//     [COL_STATE_FRONTIER] = COLOR_AMBER_YELLOW,
//     [COL_STATE_GEN_VISITED] = COLOR_PINK,
//     [COL_STATE_SOLVE_VISITED] = COLOR_PINK,
//     [COL_STATE_BACKTRACKED] = COLOR_PARCHMENT,
//     [COL_STATE_SOLUTION] = COLOR_LAPIS,
// };
//
// // --- Dark Theme completely overhauled for proper contrast and luminance ---
// static const SDL_Color DEFAULT_DARK_COLORS[TOTAL_COLORS] = {
//     [COL_RENDER_BACKGROUND] =
//         COLOR_EIGENGRAU, // Slightly offsets from pure black paths
//     [COL_STATE_GENERATED] = COLOR_BLACK,   // Path negative space
//     [COL_STATE_BACKTRACKED] = COLOR_BLACK, // Fades back to negative space
//     [COL_START_CELL] = COLOR_CORAL_RED,    // Pops on black without blinding
//     [COL_END_CELL] =
//         COLOR_MINT_GREEN, // Excellent dark mode alternative to Forest Green
//     [COL_STATE_FRONTIER] =
//         COLOR_NEON_YELLOW, // Higher visibility than Amber in dark mode
//     [COL_WALL] = COLOR_SILVER,
//     [COL_STATE_BLANK] = COLOR_CHARCOAL,
//     [COL_STATE_GEN_VISITED] = COLOR_DUSK_ROSE, // Visible but subdued
//     [COL_STATE_SOLVE_VISITED] =
//         COLOR_PLUM_PURPLE, // Distinct from generation, but clearly visible
//     [COL_STATE_SOLUTION] = COLOR_BRIGHT_CYAN, // Clean, high-contrast path
// };
