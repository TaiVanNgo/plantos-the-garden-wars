#include "../../include/zombies.h"

// Default Zombie Basic
const Zombie default_zombie_normal = {
    .type = ZOMBIE_NORMAL,
    .x = 800,
    .y = 0,
    .row = 0,
    .health = 100,
    .max_health = 100,
    .speed = 1,
    .damage = 10,
    .attack_speed = 5,
    .is_frozen = 0,
    .active = 1};

const Zombie default_zombie_bucket = {
    .type = ZOMBIE_BUCKET,
    .x = 800,
    .y = 0,
    .row = 0,
    .health = 150,
    .max_health = 150,
    .speed = 1,
    .damage = 10,
    .attack_speed = 8,
    .is_frozen = 0,
    .active = 1};

const Zombie default_zombie_helmet = {
    .type = ZOMBIE_HELMET,
    .x = 800,
    .y = 0,
    .row = 0,
    .health = 250,
    .max_health = 250,
    .speed = 1,
    .damage = 15,
    .attack_speed = 8,
    .is_frozen = 0,
    .active = 1};

Zombie create_zombie(uint8_t type, uint8_t row)
{
  Zombie new_zombie;
  switch (type)
  {
  case ZOMBIE_NORMAL:
    new_zombie = default_zombie_normal;
    break;
  case ZOMBIE_BUCKET:
    new_zombie = default_zombie_bucket;
    break;
  case ZOMBIE_HELMET:
    new_zombie = default_zombie_helmet;
    break;
  default:
    break;
  }

  new_zombie.row = row;
  new_zombie.x = START_X_POS; // Spawn right of the screen
  new_zombie.y = GRID_TOP_MARGIN + (row * GRID_ROW_HEIGHT);
  new_zombie.active = 1;
  return new_zombie;
}

Zombie spawn_zombie(uint8_t type, uint8_t row)
{
  Zombie new_zombie = create_zombie(type, row);
  switch (type)
  {

  case ZOMBIE_NORMAL:
    draw_image(ZOMBIE_NORMAL_SPRITE, new_zombie.x, new_zombie.y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0);
    break;
  case ZOMBIE_BUCKET:
    draw_image(ZOMBIE_BUCKET_SPRITE, new_zombie.x, new_zombie.y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0);
    break;
  case ZOMBIE_HELMET:
    draw_image(ZOMBIE_HELMET_SPRITE, new_zombie.x, new_zombie.y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0);
    break;
  default:
    // Fallback to normal zombie if type is unknown    draw_image(ZOMBIE_NORMAL_SPRITE, new_zombie.x, new_zombie.y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0);
    break;
  }
  return new_zombie;
}

int move_zombie(Zombie *zombie)
{
  // if zombie is dead
  if (!zombie->active)
    return 0;

  uint8_t actual_speed = zombie->is_frozen ? (zombie->speed / 2) : zombie->speed;

  // Calculate new position
  int new_x = zombie->x;
  if (zombie->x > actual_speed)
  {
    new_x -= actual_speed;
  }
  else
  {
    // Zombie reached the left most edge of the screen
    zombie->x = 0;
    return 1; // zombie reached the edge
  }

  // Update new position
  zombie->x = new_x;

  return 0;
}

void update_zombie_position(Zombie *zombie)
{
  if (!zombie->active)
  {
    return;
  }

  int old_x = zombie->x;
  int old_y = zombie->y;

  // Restore background from old position
  restore_background_area(old_x, old_y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0);

  // Move zombie left (update position)
  move_zombie(zombie);

  // Draw the zombie at its current position
  switch (zombie->type)
  {
  case ZOMBIE_NORMAL:
    draw_image(ZOMBIE_NORMAL_SPRITE, zombie->x, zombie->y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 1);
    break;
  case ZOMBIE_BUCKET:
    draw_image(ZOMBIE_BUCKET_SPRITE, zombie->x, zombie->y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 1);
    break;
  case ZOMBIE_HELMET:
    draw_image(ZOMBIE_HELMET_SPRITE, zombie->x, zombie->y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 1);
    break;
  default:
    break;
  }
}

// void update_zombie_in_global_array(Zombie *zombie)
// {
//   // Removed row check and global array update logic
//   // This function now does nothing, allowing multiple zombies to be spawned without interference
// }

// // Add new function to check if any zombie is on a given row
// int is_zombie_on_row(int row)
// {
//   // This function is no longer valid with the new implementation
//   return 0;
// }

/*//////////////////////////////////////////////////////////////
                              DEV_ONLY
//////////////////////////////////////////////////////////////*/
void dev_test_zombie()
{
  draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
  char c = getUart();

  Zombie zombie1 = spawn_zombie(1, 0);
  Zombie zombie2, zombie3, zombie4, zombie5;
  int zombie2_spawned = 0;
  int zombie3_spawned = 0;
  int zombie4_spawned = 0;
  int zombie5_spawned = 0;

  int cnt = 0;

  // Game loop running at 10FPS
  while (1)
  {
    // set_wait_timer(1, 100); // Round up to 17ms for simplicity

    update_zombie_position(&zombie1);

    if (cnt >= 50 && !zombie2_spawned)
    {
      zombie2 = spawn_zombie(2, 1);
      zombie2_spawned = 1;
    }

    if (cnt >= 100 && !zombie3_spawned)
    {
      zombie3 = spawn_zombie(3, 2);
      zombie3_spawned = 1;
    }

    if (cnt >= 150 && !zombie4_spawned)
    {
      zombie4 = spawn_zombie(1, 3);
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
    // set_wait_timer(0, 0); // Second parameter is ignored in wait mode
  }
}