#ifndef COOLDOWN_H
#define COOLDOWN_H

#include "../include/framebf.h"
#include "../include/uart0.h"
#include "../include/plants.h"
#include "../assets/backgrounds/garden.h"
#include "../include/zombies.h"
#include "../include/utils.h"

// Cooldown functions
void start_plant_cooldown(int plant_type);
int get_plant_cooldown(int plant_type);
int is_plant_on_cooldown(int plant_type);
void update_plant_cooldowns();
void display_plant_cooldown(int plant_type);
void display_all_cooldowns();

extern int plant_cooldowns[10];

#endif
