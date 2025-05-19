#ifndef ZOMBIE_H
#define ZOMBIE_H

#include "gpio.h"
#include "framebf.h"
#include "../assets/sprites/zombies/zombie_sprite.h"

// Zombie Type enum
enum ZombieType
{
  ZOMBIE_NORMAL = 1,
  ZOMBIE_BUCKET = 2,
  ZOMBIE_HELMET = 3,
};

typedef struct
{
  uint8_t type; // Zombie type (enum ZombieType)
  uint16_t x;   // X position on the grid (pixels)
  uint16_t y;   // Y position on the grid (pixels)
  uint8_t row;  // Which row (0–3 for 4 lanes)

  uint16_t health;      // Current health
  uint16_t max_health;  // Maximum health
  uint8_t speed;        // Movement speed (pixels per tick)
  uint8_t damage;       // Damage to plants per tick
  uint8_t attack_speed; // Attack rate (ticks between attacks)
  uint8_t is_frozen;    // Frozen status (1 = yes, 0 = no)
  uint8_t active;       // 1 = alive and moving, 0 = dead
} Zombie;

extern const Zombie default_zombie_normal;
Zombie create_zombie(uint8_t type, uint8_t row); // update zombie data
Zombie spawn_zombie(uint8_t type, uint8_t row);  // Draw zombie on screen
int move_zombie(Zombie *zombie);                 // Returns 1 if reached edge, 0 otherwise
void update_zombie_position(Zombie *zombie);

#endif