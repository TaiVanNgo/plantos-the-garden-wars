#ifndef GAME_INIT_H
#define GAME_INIT_H

#include "../assets/backgrounds/garden.h"
#include "zombies.h"
#include "utils.h"
#include "uart0.h"
#include "uart1.h"
#include "framebf.h"
#include "../assets/backgrounds/background.h"
#include "../assets/selection/selection.h"
#include "../assets/button/button.h"
#include "plants.h"
#include "grid.h"

#define ROWS 4
#define COLS 9
#define MAX_ZOMBIES_PER_LEVEL 20
#define MAX_ROUNDS 5
#define ZOMBIE_KILL_REWARD 10

extern int GAME_START = 0;

// Enum for game states
typedef enum
{
  GAME_MENU,
  GAME_PLAYING,
  GAME_QUIT,
  GAME_PAUSED,
  GAME_VICTORY,
  GAME_OVER
} GAME_STATE;

typedef enum
{
  LEVEL_EASY_ENUM,
  LEVEL_INTERMEDIATE_ENUM,
  LEVEL_HARD_ENUM,
} LEVEL_DIFFICULTY;

// Main game state structure
typedef struct
{
  GAME_STATE state;
  int score;
  int level;
} GameState;

typedef struct
{
  int zombie_count;                        // total zombies in a level
  int zombie_types[MAX_ZOMBIES_PER_LEVEL]; // type of each zombie (normal, bucket, helmet)
  int zombie_rows[MAX_ZOMBIES_PER_LEVEL];  // the row that zombie appears
  int spawn_times[MAX_ZOMBIES_PER_LEVEL];  // spawn time for each zombie
} Level;

const Level LEVEL_EASY = {
    .zombie_count = 5,
    .zombie_types = {ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_BUCKET, ZOMBIE_HELMET},
    .zombie_rows = {0, 1, 2, 3, 1},
    .spawn_times = {0, 300, 600, 900, 1200},
};

const Level LEVEL_INTERMEDIATE = {
    .zombie_count = 8,
    .zombie_types = {ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_BUCKET, ZOMBIE_NORMAL, ZOMBIE_BUCKET, ZOMBIE_NORMAL, ZOMBIE_BUCKET},
    .zombie_rows = {0, 1, 2, 3, 1, 0, 2, 3},
    .spawn_times = {0, 200, 400, 600, 800, 1000, 1200, 1400},
};

void game_init();
GAME_STATE game_menu();
void start_level(GameState *game, LEVEL_DIFFICULTY difficulty);

#endif
