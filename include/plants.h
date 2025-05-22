#ifndef PLANTS_H
#define PLANTS_H

#include "../assets/sprites/plants/plants_sprites.h"
#include "../assets/backgrounds/garden.h"
#include "framebf.h"
#include "grid.h"

// Define plant types
#define PLANT_TYPE_SUNFLOWER          1
#define PLANT_TYPE_PEASHOOTER         2
#define PLANT_TYPE_FROZEN_PEASHOOTER  3
#define PLANT_TYPE_WALLNUT            4
#define PLANT_TYPE_CHILLIES           5
#define PLANT_TYPE_SUNFLOWER_UNHAPPY  6
#define PLANT_TYPE_CHILLIES_UNHAPPY   7
#define PLANT_TYPE_WALLNUT_UNHAPPY    8
#define SHOVEL                        9
// Plant dimensions
#define PLANT_WIDTH              70   // Default plant sprite width
#define PLANT_HEIGHT             70   // Default plant sprite height

enum PlantType
{
    PLANT_SUNFLOWER = 1,
    PLANT_PEASHOOTER = 2,
    PLANT_FROZEN_PEASHOOTER = 3,
    PLANT_WALNUT = 4,
    PLANT_CHILLIES = 5
};

typedef struct
{
    uint8_t type;          // Plant type (using enum PlantType)
    uint8_t health;        // Current health points
    uint8_t max_health;    // Maximum health points
    uint8_t col;           // Column position on the game grid (0-based)
    uint8_t row;           // Row position on the game grid (0-based)
    uint8_t attack_damage; // Damage dealt to zombies
    uint8_t cost;          // Resource cost to plant
    uint8_t attack_speed;  // Attacks per time unit
} Plant;

void draw_plant(int plant_type, int col, int row);
Plant create_plant(uint8_t type, uint8_t col, uint8_t row);
void fill_plant_grid(void);
int get_plant_damage(int plant_type);
void place_plant_on_background(int plant_type, int grid_col, int grid_row, unsigned int *sim_bg);
#endif 