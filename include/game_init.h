#ifndef GAME_INIT_H
#define GAME_INIT_H

#define ROWS 4
#define COLS 9

extern int GAME_START =0 ;

#include "../assets/backgrounds/garden.h"
#include "zombies.h"
#include "utils.h"

// Enum for game states
typedef enum
{
  GAME_MENU,
  GAME_PLAYING,
  GAME_PAUSED,
  GAME_OVER
} GAME_STATE;

// Main game state structure
typedef struct
{
  GAME_STATE state;
  int score;
  int round;
} GameState;

void game_init();
void game_start();

#endif
