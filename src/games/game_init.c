#include "../../include/game_init.h"

void game_init()
{
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
    }
    if (zombie2_spawned)
    {
      update_zombie_position(&zombie2);
    }
    cnt++;
    delay_ms(100);
  }
}