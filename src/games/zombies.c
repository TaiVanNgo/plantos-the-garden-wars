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

Zombie create_zombie(uint8_t type, uint8_t row)
{
  Zombie new_zombie;
  switch (type)
  {
  case ZOMBIE_NORMAL:
    new_zombie = default_zombie_normal;
    break;
  // Add other types here
  default:
    break;
  }

  int lane_height = 96;
  int top_margin = 80; // Pixels from top of screen to first lane

  new_zombie.row = row;
  new_zombie.x = 700; // Spawn from right
  new_zombie.y = top_margin + (row * lane_height);
  new_zombie.active = 1;
  return new_zombie;
}

Zombie spawn_zombie(uint8_t type, uint8_t row)
{
  Zombie new_zombie = create_zombie(type, row);

  draw_image(ZOMBIE_NORMAL_SPRITE, new_zombie.x, new_zombie.y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0);

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
  restore_background_area(old_x, old_y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT);

  // Move zombie left (update position)
  move_zombie(zombie);

  // Draw the zombie at its current position
  switch (zombie->type)
  {
  case ZOMBIE_NORMAL:
    draw_image(ZOMBIE_NORMAL_SPRITE, zombie->x, zombie->y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0);
    break;
  case ZOMBIE_BUCKET:
    // draw_image(bucket_zombie, zombie->x, zombie->y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0);
    break;
  case ZOMBIE_HELMET:
    // draw_image(helmet_zombie, zombie->x, zombie->y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0);
    break;
  default:
    break;
  }
}