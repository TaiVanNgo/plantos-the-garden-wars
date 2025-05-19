#ifndef PLANTS_H
#define PLANTS_H

#include "../assets/sprites/plants/plants_sprites.h"
#include "framebf.h"

// Define plant types
#define PLANT_TYPE_PEASHOOTER         0
#define PLANT_TYPE_SUNFLOWER          1
#define PLANT_TYPE_SUNFLOWER_UNHAPPY  2
#define PLANT_TYPE_FROZEN_PEASHOOTER  3
#define PLANT_TYPE_CHILLIES           4
#define PLANT_TYPE_CHILLIES_UNHAPPY   5
#define PLANT_TYPE_WALLNUT            6
#define PLANT_TYPE_WALLNUT_UNHAPPY    7

// Grid parameters
#define PLANT_GRID_LEFT_MARGIN   50   // Starting x position for the first column
#define PLANT_GRID_TOP_MARGIN    180  // Starting y position for the first row
#define PLANT_COL_WIDTH          70   // Width of each column
#define PLANT_ROW_HEIGHT         85   // Height of each row
#define PLANT_GRID_ROWS          4    // Number of rows in grid
#define PLANT_GRID_COLS          9    // Number of columns in grid

// Plant dimensions
#define PLANT_WIDTH              70   // Default plant sprite width
#define PLANT_HEIGHT             70   // Default plant sprite height

// Function to draw a plant at the specified grid position
void draw_plant(int plant_type, int col, int row);

// Function to draw the grid
void draw_plant_grid(void);

#endif /* PLANTS_H */