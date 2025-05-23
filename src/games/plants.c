#include "../../include/plants.h"
#include "../../include/grid.h"
#include "../assets/button/button.h"
#include "gpio.h"

// Default Sunflower
const Plant default_sunflower = {
    .type = PLANT_SUNFLOWER,
    .health = 50,
    .max_health = 50,
    .col = 0,
    .row = 0,
    .attack_damage = 0, // Sunflowers don't attack
    .cost = 50,
    .attack_speed = 0, // Sunflowers don't attack
};

// Default Peashooter
const Plant default_peashooter = {
    .type = PLANT_PEASHOOTER,
    .health = 100,
    .max_health = 100,
    .col = 0,
    .row = 0,
    .attack_damage = 20,
    .cost = 100,
    .attack_speed = 5, // Fires 5 peas per time unit
};

// Default Frozen Peashooter
const Plant default_frozen_peashooter = {
    .type = PLANT_FROZEN_PEASHOOTER,
    .health = 100,
    .max_health = 100,
    .col = 0,
    .row = 0,
    .attack_damage = 15, // Slightly less damage than regular peashooter
    .cost = 175,         // More expensive due to freezing ability
    .attack_speed = 4,   // Fires 4 peas per time unit
};

// Default Walnut
const Plant default_walnut = {
    .type = PLANT_WALNUT,
    .health = 250,
    .max_health = 250,
    .col = 0,
    .row = 0,
    .attack_damage = 0, // Walnuts don't attack
    .cost = 50,
    .attack_speed = 0, // Walnuts don't attack
};

// Default Chillies
const Plant default_chillies = {
    .type = PLANT_CHILLIES,
    .health = 50,
    .max_health = 50,
    .col = 0,
    .row = 0,
    .attack_damage = 150, // High damage as it's an explosive plant
    .cost = 125,
    .attack_speed = 1, // One-time use, explodes once
};

// Function to create a new plant of the specified type
Plant create_plant(uint8_t type, uint8_t col, uint8_t row)
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

    // Set the grid position
    new_plant.col = col;
    new_plant.row = row;

    return new_plant;
}

void draw_plant(int plant_type, int col, int row)
{
    if (!is_valid_grid_position(col, row))
    {
        return;
    }

    // Calculate pixel coordinates from grid position, centered in cell
    int x, y;
    grid_to_pixel(col, row, &x, &y);

    // Get offsets to center the plant in the cell
    int offset_x, offset_y;
    calculate_grid_center_offset(PLANT_WIDTH, PLANT_HEIGHT, &offset_x, &offset_y);

    // Apply the offsets
    x += offset_x;
    y += offset_y;

    const unsigned int *plant_sprite;

    switch (plant_type)
    {
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
    case SHOVEL:
        plant_sprite = shovel;
        break;
    default:
        return;
    }

    draw_image(plant_sprite, x, y, PLANT_WIDTH, PLANT_HEIGHT, 0);
}

// Fill the plant grid for testing purposes
void fill_plant_grid(void)
{
    int plant_types[] = {
        PLANT_TYPE_PEASHOOTER,
        PLANT_TYPE_SUNFLOWER,
        PLANT_TYPE_SUNFLOWER_UNHAPPY,
        PLANT_TYPE_FROZEN_PEASHOOTER,
        PLANT_TYPE_CHILLIES,
        PLANT_TYPE_CHILLIES_UNHAPPY,
        PLANT_TYPE_WALLNUT,
        PLANT_TYPE_WALLNUT_UNHAPPY};
    int num_types = sizeof(plant_types) / sizeof(plant_types[0]);
    int type_idx = 0;

    for (int y = 0; y < GRID_ROWS; y++)
    {
        for (int x = 0; x < GRID_COLS; x++)
        {
            draw_plant(plant_types[type_idx], x, y);
            type_idx = (type_idx + 1) % num_types;
        }
    }
}

int get_plant_damage(int plant_type)
{
    switch (plant_type)
    {
    case PLANT_TYPE_PEASHOOTER:
        return default_peashooter.attack_damage;
    case PLANT_TYPE_FROZEN_PEASHOOTER:
        return default_frozen_peashooter.attack_damage;
    case PLANT_TYPE_CHILLIES:
        return default_chillies.attack_damage;
    default:
        return 0;
    }
}

void place_plant_on_background(int plant_type, int grid_col, int grid_row, unsigned int *sim_bg)
{
    const unsigned int *plant;

    switch (plant_type)
    {
    case PLANT_TYPE_PEASHOOTER:
        plant = peashooter;
        break;
    case PLANT_TYPE_SUNFLOWER:
        plant = sunflower;
        break;
    case PLANT_TYPE_SUNFLOWER_UNHAPPY:
        plant = sunflower_unhappy;
        break;
    case PLANT_TYPE_FROZEN_PEASHOOTER:
        plant = frozen_peashooter;
        break;
    case PLANT_TYPE_CHILLIES:
        plant = chillies;
        break;
    case PLANT_TYPE_CHILLIES_UNHAPPY:
        plant = chillies_unhappy;
        break;
    case PLANT_TYPE_WALLNUT:
        plant = wallnut;
        break;
    case PLANT_TYPE_WALLNUT_UNHAPPY:
        plant = wallnut_unhappy;
        break;
    default:
        uart_puts("Invalid plant type!\n");
        return;
    }

    int x, y;
    grid_to_pixel(grid_col, grid_row, &x, &y);

    // Get offsets to center the plant in the cell
    int offset_x, offset_y;
    calculate_grid_center_offset(PLANT_WIDTH, PLANT_HEIGHT, &offset_x, &offset_y);

    // Apply the offsets
    x += offset_x;
    y += offset_y;

    // Draw on both the screen and simulated background
    draw_on_simulated_background(sim_bg, plant, x, y, PLANT_WIDTH, PLANT_HEIGHT, GARDEN_WIDTH);
    draw_image(plant, x, y, PLANT_WIDTH, PLANT_HEIGHT, 0);
}

// get plant name
const char *get_plant_name(int type)
{
    switch (type)
    {
    case PLANT_TYPE_SUNFLOWER:
        return "Sunflower";
    case PLANT_TYPE_PEASHOOTER:
        return "Peashooter";
    case PLANT_TYPE_FROZEN_PEASHOOTER:
        return "Snow Pea";
    case PLANT_TYPE_WALLNUT:
        return "Wall-nut";
    case PLANT_TYPE_CHILLIES:
        return "Chilies";
    case SHOVEL:
        return "Shovel";
    default:
        return "Unknown";
    }
}