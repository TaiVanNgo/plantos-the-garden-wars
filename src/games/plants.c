#include "../../include/plants.h"
#include "gpio.h"

// Plant type enum
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
    uint16_t x;            // X position on the game grid
    uint16_t y;            // Y position on the game grid
    uint8_t attack_damage; // Damage dealt to zombies
    uint8_t cost;          // Resource cost to plant
    uint8_t attack_speed;  // Attacks per time unit
    uint8_t attack_range;  // Range of attack in grid units
} Plant;

// Default Sunflower
const Plant default_sunflower = {
    .type = PLANT_SUNFLOWER,
    .health = 50,
    .max_health = 50,
    .x = 0,
    .y = 0,
    .attack_damage = 0, // Sunflowers don't attack
    .cost = 50,
    .attack_speed = 0, // Sunflowers don't attack
    .attack_range = 0, // Sunflowers don't attack
};

// Default Peashooter
const Plant default_peashooter = {
    .type = PLANT_PEASHOOTER,
    .health = 100,
    .max_health = 100,
    .x = 0,
    .y = 0,
    .attack_damage = 20,
    .cost = 100,
    .attack_speed = 5, // Fires 5 peas per time unit
    .attack_range = 5, // Can attack zombies in the same row up to 5 tiles away
};

// Default Frozen Peashooter
const Plant default_frozen_peashooter = {
    .type = PLANT_FROZEN_PEASHOOTER,
    .health = 100,
    .max_health = 100,
    .x = 0,
    .y = 0,
    .attack_damage = 15, // Slightly less damage than regular peashooter
    .cost = 175,         // More expensive due to freezing ability
    .attack_speed = 4,   // Fires 4 peas per time unit
    .attack_range = 5,   // Can attack zombies in the same row up to 5 tiles away
};

// Default Walnut
const Plant default_walnut = {
    .type = PLANT_WALNUT,
    .health = 400, // High health as it's a defensive plant
    .max_health = 400,
    .x = 0,
    .y = 0,
    .attack_damage = 0, // Walnuts don't attack
    .cost = 50,
    .attack_speed = 0, // Walnuts don't attack
    .attack_range = 0, // Walnuts don't attack
};

// Default Chillies
const Plant default_chillies = {
    .type = PLANT_CHILLIES,
    .health = 50,
    .max_health = 50,
    .x = 0,
    .y = 0,
    .attack_damage = 150, // High damage as it's an explosive plant
    .cost = 125,
    .attack_speed = 1, // One-time use, explodes once
    .attack_range = 2, // Affects an area of 2 tiles in all directions
};

// Function to create a new plant of the specified type
Plant create_plant(uint8_t type, uint16_t x, uint16_t y)
{
    Plant new_plant;

    switch (type)
    {
    case PLANT_SUNFLOWER:
        new_plant = default_sunflower;
        break;
    case PLANT_PEASHOOTER:
        new_plant = default_peashooter;
        break;
    case PLANT_FROZEN_PEASHOOTER:
        new_plant = default_frozen_peashooter;
        break;
    case PLANT_WALNUT:
        new_plant = default_walnut;
        break;
    case PLANT_CHILLIES:
        new_plant = default_chillies;
        break;
    default:
        break;
    }

    // Set the position
    new_plant.x = x;
    new_plant.y = y;

    return new_plant;
}

/**
 * Draw a plant at the specified grid position
 * 
 * @param plant_type The type of plant to draw (use PLANT_TYPE constants)
 * @param col Column in the grid (0-based)
 * @param row Row in the grid (0-based, corresponds to lanes)
 */
void draw_plant(int plant_type, int col, int row) {
    // Check if the column and row are within valid bounds
    if (col < 0 || col >= PLANT_GRID_COLS || row < 0 || row >= PLANT_GRID_ROWS) {
        return; // Invalid position, do nothing
    }
    
    // Calculate pixel coordinates from grid position, centered in cell
    int horizontal_offset = (PLANT_COL_WIDTH - PLANT_WIDTH) / 2;
    int vertical_offset = (PLANT_ROW_HEIGHT - PLANT_HEIGHT) / 2;
    int x = PLANT_GRID_LEFT_MARGIN + (col * PLANT_COL_WIDTH) + horizontal_offset;
    int y = PLANT_GRID_TOP_MARGIN + (row * PLANT_ROW_HEIGHT) + vertical_offset;
    
    const unsigned int* plant_sprite;
    
    switch(plant_type) {
        case PLANT_TYPE_PEASHOOTER:
            plant_sprite = peashooter;
            break;
        case PLANT_TYPE_SUNFLOWER:
            plant_sprite = sunflower;
            break;
        case PLANT_TYPE_SUNFLOWER_UNHAPPY:
            plant_sprite = sunflower_unhappy;
            break;
        case PLANT_TYPE_FROZEN_PEASHOOTER:
            plant_sprite = frozen_peashooter;
            break;
        case PLANT_TYPE_CHILLIES:
            plant_sprite = chillies;
            break;
        case PLANT_TYPE_CHILLIES_UNHAPPY:
            plant_sprite = chillies_unhappy;
            break;
        case PLANT_TYPE_WALLNUT:
            plant_sprite = wallnut;
            break;
        case PLANT_TYPE_WALLNUT_UNHAPPY:
            plant_sprite = wallnut_unhappy;
            break;
        default:
            // Handle invalid plant type
            return;
    }
    
    draw_image(plant_sprite, x, y, PLANT_WIDTH, PLANT_HEIGHT, 0);
}

/**
 * Draw the plant grid with gridlines and margins
 */
void draw_plant_grid(void) {
    unsigned int grid_color = 0x002299FF;    
    unsigned int margin_color = 0x00FF0000;  
    
    // Draw horizontal grid lines
    for (int i = 0; i <= PLANT_GRID_ROWS; i++) {
        draw_line(
            PLANT_GRID_LEFT_MARGIN, 
            PLANT_GRID_TOP_MARGIN + (i * PLANT_ROW_HEIGHT), 
            GARDEN_WIDTH, 
            PLANT_GRID_TOP_MARGIN + (i * PLANT_ROW_HEIGHT), 
            grid_color
        );
    }
    
    // Top margin (horizontal line)
    for (int i = 0; i < 3; i++) {
        draw_line(
            0, 
            PLANT_GRID_TOP_MARGIN + i, 
            GARDEN_WIDTH, 
            PLANT_GRID_TOP_MARGIN + i, 
            margin_color
        );
    } 
}

// Fill the plant grid for testing purposes
void fill_plant_grid(void) {
    int plant_types[] = {
        PLANT_TYPE_PEASHOOTER,
        PLANT_TYPE_SUNFLOWER,
        PLANT_TYPE_SUNFLOWER_UNHAPPY,
        PLANT_TYPE_FROZEN_PEASHOOTER,
        PLANT_TYPE_CHILLIES,
        PLANT_TYPE_CHILLIES_UNHAPPY,
        PLANT_TYPE_WALLNUT,
        PLANT_TYPE_WALLNUT_UNHAPPY
    };
    int num_types = sizeof(plant_types) / sizeof(plant_types[0]);
    int cols = 9;
    int rows = 4;
    int type_idx = 0;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            draw_plant(plant_types[type_idx], x, y);
            type_idx = (type_idx + 1) % num_types;
        }
    }

}