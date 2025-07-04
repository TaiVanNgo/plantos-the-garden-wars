#ifndef GAME_INIT_H
#define GAME_INIT_H

#include "../assets/backgrounds/garden.h"
#include "../assets/selection/selection.h"
#include "bullet.h"
#include "buttons.h"
#include "cooldown.h"
#include "framebf.h"
#include "grid.h"
#include "plants.h"
#include "sun.h"
#include "uart0.h"
#include "uart1.h"
#include "utils.h"
#include "zombies.h"

#define ROWS 4
#define COLS 9
#define MAX_ZOMBIES_PER_LEVEL 20
#define ZOMBIE_KILL_REWARD 10

// Enum for game states
typedef enum {
    GAME_MENU,
    GAME_DIFFICULTY,
    GAME_PLAYING,
    GAME_QUIT,
    GAME_VICTORY,
    GAME_OVER
} GAME_STATE;

typedef enum {
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
    int mode;           // 0 = card selection, 1 = grid placement
    int selected_card;  // currently selected plant card
    int row;            // current row
    int col;            // current col
    int current_plant;  // current selected plant type
} SelectionState;

typedef struct
{
    int zombie_count;                         // total zombies in a level
    int zombie_types[MAX_ZOMBIES_PER_LEVEL];  // type of each zombie (normal, bucket, helmet)
    int zombie_rows[MAX_ZOMBIES_PER_LEVEL];   // the row that zombie appears
    int spawn_times[MAX_ZOMBIES_PER_LEVEL];   // spawn time for each zombie
} Level;

// External global variables
extern GameState game;
extern SelectionState select_state;

void game_main();
void game_menu();
void game_start_difficulty();
void start_level();
void game_over();
int handle_user_input(int *frame_counter);
int check_occupied();
void handle_plant_selection(int plant_type, int current_frame);
void handle_remove_plant();
void handle_arrow_keys();
void handle_enter_key(int frame_counter);
void victory_screen();
void set_zombie_level_config(int level, int zombie_types[10], int spawn_times[10]);
void draw_cursor();
void draw_all_plant_cooldowns();
void draw_control_instructions();

#endif
