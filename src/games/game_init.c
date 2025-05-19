#include "../../include/game_init.h"

void game_init()
{
  draw_image(GARDEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
  draw_rect(0, 70, 800, 70, RED, 0);
  draw_rect(0, 184, 800, 272, BLUE, 0);

  Zombie zombie1 = spawn_zombie(1, 1);
  Zombie zombie2;
  int zombie2_spawned = 0;

  int cnt = 0;
  while (1)
  {

    update_zombie_position(&zombie1);
    if (cnt >= 50 && !zombie2_spawned)
    {
      zombie2 = spawn_zombie(1, 2);
      zombie2_spawned = 1;
      Zombie zombie3 = spawn_zombie(1, 3);
      Zombie zombie4 = spawn_zombie(1, 4);
    }
    if (zombie2_spawned)
    {
      update_zombie_position(&zombie2);
    }
    cnt++;
    delay_ms(100);
  }
}