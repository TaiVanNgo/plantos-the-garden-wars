#ifndef GAME_INIT_H
#define GAME_INIT_H

#include "../include/bullet.h"
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
#define ZOMBIE_KILL_REWARD 10

// Enum for game states
typedef enum
{
  GAME_MENU,
  GAME_DIFFICULTY,
  GAME_PLAYING,
  GAME_QUIT,
  GAME_PAUSED,
  GAME_VICTORY,
  GAME_OVER
} GAME_STATE;

typedef enum
{
  LEVEL_EASY_ENUM,
  LEVEL_MEDIUM_ENUM,
  LEVEL_HARD_ENUM,
} LEVEL_DIFFICULTY;

// Main game state structure
typedef struct
{
  GAME_STATE state;
  int score;
  LEVEL_DIFFICULTY level;
  int sun_count; 
} GameState;

typedef struct
{
  int mode;          // 0 = card selection, 1 = grid placement
  int selected_card; // currently selected plant card
  int row;           // current row
  int col;           // current col
  int current_plant; // current selected plant type
} SelectionState;

typedef struct
{
  int zombie_count;                        // total zombies in a level
  int zombie_types[MAX_ZOMBIES_PER_LEVEL]; // type of each zombie (normal, bucket, helmet)
  int zombie_rows[MAX_ZOMBIES_PER_LEVEL];  // the row that zombie appears
  int spawn_times[MAX_ZOMBIES_PER_LEVEL];  // spawn time for each zombie
} Level;

// External global variables
extern GameState game;
extern SelectionState select_state;
extern Plant plant_grid[GRID_ROWS][GRID_COLS];

void game_main();
void game_menu();
void start_level();
void game_over();
int handle_user_input(int *frame_counter);
int check_occupied();
void handle_plant_selection(int plant_type);
void handle_remove_plant();
void handle_arrow_keys();
void handle_enter_key();
void victory_screen();
int get_selection_current_plant(void);
int get_selection_row(void);
int get_selection_col(void);

#endif
