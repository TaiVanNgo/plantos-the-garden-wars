#include "gpio.h"

// Plant type enum
enum PlantType {
    PLANT_SUNFLOWER = 1,
    PLANT_PEASHOOTER = 2,
    PLANT_FROZEN_PEASHOOTER = 3,
    PLANT_WALNUT = 4,
    PLANT_CHILLIES = 5
};

typedef struct {
    uint8_t type;        // Plant type (using enum PlantType)
    uint8_t health;      // Current health points
    uint8_t max_health;  // Maximum health points
    uint16_t x;          // X position on the game grid
    uint16_t y;          // Y position on the game grid
    uint8_t attack_damage; // Damage dealt to zombies
    uint8_t cost;        // Resource cost to plant
    uint8_t attack_speed; // Attacks per time unit
    uint8_t attack_range; // Range of attack in grid units
} Plant;

// Default Sunflower
const Plant default_sunflower = {
    .type = PLANT_SUNFLOWER,
    .health = 50,
    .max_health = 50,
    .x = 0,
    .y = 0,
    .attack_damage = 0,     // Sunflowers don't attack
    .cost = 50,
    .attack_speed = 0,      // Sunflowers don't attack
    .attack_range = 0,      // Sunflowers don't attack
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
    .attack_speed = 5,      // Fires 5 peas per time unit
    .attack_range = 5,      // Can attack zombies in the same row up to 5 tiles away
};

// Default Frozen Peashooter
const Plant default_frozen_peashooter = {
    .type = PLANT_FROZEN_PEASHOOTER,
    .health = 100,
    .max_health = 100,
    .x = 0,
    .y = 0,
    .attack_damage = 15,    // Slightly less damage than regular peashooter
    .cost = 175,            // More expensive due to freezing ability
    .attack_speed = 4,      // Fires 4 peas per time unit
    .attack_range = 5,      // Can attack zombies in the same row up to 5 tiles away
};

// Default Walnut
const Plant default_walnut = {
    .type = PLANT_WALNUT,
    .health = 400,          // High health as it's a defensive plant
    .max_health = 400,
    .x = 0,
    .y = 0,
    .attack_damage = 0,     // Walnuts don't attack
    .cost = 50,
    .attack_speed = 0,      // Walnuts don't attack
    .attack_range = 0,      // Walnuts don't attack
};

// Default Chillies
const Plant default_chillies = {
    .type = PLANT_CHILLIES,
    .health = 50,
    .max_health = 50,
    .x = 0,
    .y = 0,
    .attack_damage = 150,   // High damage as it's an explosive plant
    .cost = 125,
    .attack_speed = 1,      // One-time use, explodes once
    .attack_range = 2,      // Affects an area of 2 tiles in all directions
};

// Function to create a new plant of the specified type
Plant create_plant(uint8_t type, uint16_t x, uint16_t y) {
    Plant new_plant;
    
    switch(type) {
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
            // TODO: handle invalid type
            break;
    }
    
    // Set the position 
    new_plant.x = x;
    new_plant.y = y;
    
    return new_plant;
}