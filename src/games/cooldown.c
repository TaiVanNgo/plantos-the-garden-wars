#include "../include/cooldown.h"
#include "../include/game_init.h"  // Add this for select_state

// Track cooldowns for each plant type
int plant_cooldowns[10] = {0};
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

                // Clear the cooldown overlay by restoring the background
                const int FIRST_CARD_X = 65;
                const int CARDS_Y = 100;
                const int CARD_SPACING = 55;
                int card_x = FIRST_CARD_X + (i - 1) * CARD_SPACING;
                
                // Restore the background where the cooldown overlay was
                restore_background_area(card_x, CARDS_Y, 50, 70, 0, 1, 0, 0);
            }
        }
    }
}





