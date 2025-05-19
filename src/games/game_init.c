#include "../../include/game_init.h"

void game_init()
{
  draw_image(GARDEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
  draw_rect(0, 70, 800, 70, RED, 0);
  draw_rect(0, 184, 800, 272, BLUE, 0);

  Zombie zombie1 = spawn_zombie(1, 1);
  Zombie zombie2, zombie3, zombie4, zombie5;
  int zombie2_spawned = 0;
  int zombie3_spawned = 0;
  int zombie4_spawned = 0;
  int zombie5_spawned = 0;

  int cnt = 0;

  // Game loop running at 10FPS
  while (1)
  {
    set_wait_timer(1, 17); // Round up to 17ms for simplicity

    update_zombie_position(&zombie1);

    if (cnt >= 50 && !zombie2_spawned)
    {
      zombie2 = spawn_zombie(2, 2);
      zombie2_spawned = 1;
    }

    if (cnt >= 100 && !zombie3_spawned)
    {
      zombie3 = spawn_zombie(3, 3);
      zombie3_spawned = 1;
    }

    if (cnt >= 150 && !zombie4_spawned)
    {
      zombie4 = spawn_zombie(1, 4);
      zombie4_spawned = 1;
    }

    if (cnt >= 200 && !zombie5_spawned)
    {
      zombie5 = spawn_zombie(2, 1);
      zombie5_spawned = 1;
    }

    if (zombie2_spawned)
    {
      update_zombie_position(&zombie2);
    }

    if (zombie3_spawned)
    {
      update_zombie_position(&zombie3);
    }

    if (zombie4_spawned)
    {
      update_zombie_position(&zombie4);
    }

    if (zombie5_spawned)
    {
      update_zombie_position(&zombie5);
    }
    cnt++;

    // Wait until the 100ms timer expires
    set_wait_timer(0, 0); // Second parameter is ignored in wait mode
  }
}