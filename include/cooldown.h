#ifndef COOLDOWN_H
#define COOLDOWN_H

#include "../assets/backgrounds/garden.h"
#include "framebf.h"
#include "plants.h"
#include "uart0.h"
#include "utils.h"
#include "zombies.h"

// Cooldown functions
void start_plant_cooldown(int plant_type);
int get_plant_cooldown(int plant_type);
int is_plant_on_cooldown(int plant_type);
void update_plant_cooldowns();
void display_plant_cooldown(int plant_type);
void display_all_cooldowns();

extern int plant_cooldowns[10];

#endif
