#include "../include/cooldown.h"

// Track cooldowns for each plant type
static int plant_cooldowns[10] = {0};
static int display_timer = 0;  // Timer for periodic display

// Get cooldown time for a plant type (in frames)
int get_plant_cooldown(int plant_type) {
    switch (plant_type) {
        case PLANT_SUNFLOWER:
            return 120;  // 2 seconds at 60fps
        case PLANT_PEASHOOTER:
            return 90;   // 1.5 seconds
        case PLANT_FROZEN_PEASHOOTER:
            return 120;  // 2 seconds
        case PLANT_WALNUT:
            return 150;  // 2.5 seconds
        case PLANT_CHILLIES:
            return 180;  // 3 seconds
        default:
            return 0;
    }
}

// Start cooldown for a plant type
void start_plant_cooldown(int plant_type) {
    if (plant_type >= 1 && plant_type <= 5) {  // Only for actual plants
        plant_cooldowns[plant_type] = get_plant_cooldown(plant_type);
        if (plant_cooldowns[plant_type] <= 0) {
            plant_cooldowns[plant_type] = 0;  // Skip cooldown if zero or negative
            return;
        }
        uart_puts("Started cooldown for ");
        const char* plant_names[] = {
            "None",
            "Sunflower",
            "Peashooter",
            "Frozen Peashooter",
            "Wall-nut",
            "Chillies"
        };
        uart_puts(plant_names[plant_type]);
        uart_puts("\n");
    }
}

// Check if a plant is on cooldown
int is_plant_on_cooldown(int plant_type) {
    if (plant_type < 1 || plant_type > 5) return 0;
    return plant_cooldowns[plant_type] > 0;
}

// Display remaining cooldown for a plant
void display_plant_cooldown(int plant_type) {
    if (plant_type < 1 || plant_type > 5) return;
    
    const char* plant_names[] = {
        "None",
        "Sunflower",
        "Peashooter",
        "Frozen Peashooter",
        "Wall-nut",
        "Chillies"
    };
    
    uart_puts(plant_names[plant_type]);
    uart_puts(" cooldown: ");
    uart_dec(plant_cooldowns[plant_type] / 60);  // Show in seconds
    uart_puts(" seconds\n");
}

// Display all active cooldowns
void display_all_cooldowns() {
    uart_puts("\n--- Active Cooldowns ---\n");
    for (int i = 1; i <= 5; i++) {
        if (plant_cooldowns[i] > 0) {
            display_plant_cooldown(i);
        }
    }
    uart_puts("----------------------\n");
}


void update_plant_cooldowns() {
    // Update display timer
    display_timer++;
    if (display_timer >= 60) {  
        display_all_cooldowns();
        display_timer = 0;
    }

    // Update cooldowns
    for (int i = 1; i <= 5; i++) {  
        if (plant_cooldowns[i] > 0) {
            plant_cooldowns[i]--;
            if (plant_cooldowns[i] <= 0) {  
                plant_cooldowns[i] = 0;  
                uart_puts("Cooldown finished for ");
                const char* plant_names[] = {
                    "None",
                    "Sunflower",
                    "Peashooter",
                    "Frozen Peashooter",
                    "Wall-nut",
                    "Chillies"
                };
                uart_puts(plant_names[i]);
                uart_puts("\n");
            }
        }
    }
}





