#include "../include/bullet.h"

extern unsigned char *fb;
extern unsigned int simulated_background[GARDEN_WIDTH * GARDEN_HEIGHT];

// --- Static State ---
static Bullet bullets[MAX_BULLETS];
static PlantInstance plants[MAX_PLANTS];
static int plant_count = 0;
static unsigned int bullet_fire_interval = 1000; // ms, default 1 second
static unsigned int bullet_move_interval = 30;   // ms
static unsigned long last_bullet_move_time = 0;
static int target_x, target_y;
static int bullet_speed = 3;
static int game_over;
static unsigned int background_buffer[MAX_BULLETS][BULLET_WIDTH * BULLET_HEIGHT];
static int zombies_on_row[GRID_ROWS] = {0, 0, 0, 0};

// --- Background/Utility Helpers ---
static void save_background(int x, int y, int index);
static void restore_background(int x, int y, int index);
static void clear_bullet_area();
static void fire_bullet_for_plant(int col, int row);
static int bullet_should_fire(unsigned long last_fire_time, unsigned long current_time, unsigned int interval);
static int is_living_zombie_on_row(int row);

// --- Bullet System API ---
// Initialize bullet system
void bullet_system_init(unsigned long start_ms, int fire_interval_ms) {
    for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = 0;
    plant_count = 0;
    bullet_fire_interval = fire_interval_ms;
    last_bullet_move_time = start_ms;
    target_x = PHYSICAL_WIDTH - 50;
    target_y = GRID_TOP_MARGIN + GRID_ROW_HEIGHT;
    game_over = 0;
}

// Register a plant for bullet firing
void bullet_spawn_plant(int col, int row, unsigned long start_ms) {
    if (plant_count < MAX_PLANTS) {
        plants[plant_count].col = col;
        plants[plant_count].row = row;
        plants[plant_count].last_fire_time = start_ms;
        plant_count++;
        uart_puts("Plant spawned\n");
    }
}

// Fire a bullet from a plant's position
static void fire_bullet_for_plant(int col, int row) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            int shooter_x = GRID_LEFT_MARGIN + (col * GRID_COL_WIDTH) + ((GRID_COL_WIDTH - PLANT_WIDTH) / 2) + PLANT_WIDTH;
            int shooter_y = GRID_TOP_MARGIN + (row * GRID_ROW_HEIGHT) + ((GRID_ROW_HEIGHT - PLANT_HEIGHT) / 2) + (PLANT_HEIGHT / 2) - (BULLET_HEIGHT / 2);
            bullets[i].x = shooter_x;
            bullets[i].y = shooter_y;
            bullets[i].prev_x = bullets[i].x;
            bullets[i].prev_y = bullets[i].y;
            bullets[i].row = row;
            bullets[i].active = 1;
            wait_msec(1);
            bullets[i].plant_type = PLANT_TYPE_PEASHOOTER;
            
            save_background(bullets[i].x, bullets[i].y, i);
            break;
        }
    }
}

// Returns 1 if enough time has passed to fire, 0 otherwise
static int bullet_should_fire(unsigned long last_fire_time, unsigned long current_time, unsigned int interval) {
    return (current_time - last_fire_time) >= interval;
}

// Helper function to check if there is a living zombie on a row
static int is_living_zombie_on_row(int row) {
    if (row < 0 || row >= GRID_ROWS) return 0; 
    return zombies_on_row[row] > 0;
}

// Register an active zombie on a row
void register_zombie_on_row(int row, int active) {
    if (row < 0 || row >= GRID_ROWS) return; 
    if (active) {
        zombies_on_row[row]++; 
        uart_puts("Zombie added to row ");
        uart_dec(row);
        uart_puts(", count: ");
        uart_dec(zombies_on_row[row]);
        uart_puts("\n");
    } else {
        if (zombies_on_row[row] > 0) {
            zombies_on_row[row]--; 
            uart_puts("Zombie removed from row ");
            uart_dec(row);
            uart_puts(", count: ");
            uart_dec(zombies_on_row[row]);
            uart_puts("\n");
        }
    }
}

void bullet_update(unsigned long current_time_ms) {
    for (int i = 0; i < plant_count; i++) {
        if (is_living_zombie_on_row(plants[i].row) && bullet_should_fire(plants[i].last_fire_time, current_time_ms, bullet_fire_interval)) {
            uart_puts("Ready to fire bullet\n");
            fire_bullet_for_plant(plants[i].col, plants[i].row);
            plants[i].last_fire_time = current_time_ms;
        }
    }
    if ((current_time_ms - last_bullet_move_time) >= bullet_move_interval) {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                bullets[i].prev_x = bullets[i].x;
                bullets[i].prev_y = bullets[i].y;
                bullets[i].x += bullet_speed;
                
                if (bullets[i].x > PHYSICAL_WIDTH - 50) {
                    bullets[i].active = 0;  
                }
                
                if (bullets[i].x > PHYSICAL_WIDTH) {
                    bullets[i].active = 0;  
                    uart_puts("Bullet out of bounds\n");
                }
            }
        }
        last_bullet_move_time = current_time_ms;
    }
}

void bullet_draw(void) {
    int current_plant = get_selection_current_plant();
    int preview_x = 0, preview_y = 0;
    int preview_width = PLANT_WIDTH, preview_height = PLANT_HEIGHT;
    int has_preview = (current_plant != -1);
    
    // Get plant preview position if active
    if (has_preview) {
        grid_to_pixel(get_selection_col(), get_selection_row(), &preview_x, &preview_y);
        int offset_x, offset_y;
        calculate_grid_center_offset(preview_width, preview_height, &offset_x, &offset_y);
        preview_x += offset_x;
        preview_y += offset_y;
    }

    // First, restore all bullet backgrounds
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].prev_x > 0 || bullets[i].prev_y > 0) {
            restore_background(bullets[i].prev_x, bullets[i].prev_y, i);
        }
    }
    
    // Then draw all active bullets
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Check if bullet overlaps with plant preview
            if (has_preview && 
                bullets[i].x < preview_x + preview_width &&
                bullets[i].x + BULLET_WIDTH > preview_x &&
                bullets[i].y < preview_y + preview_height &&
                bullets[i].y + BULLET_HEIGHT > preview_y) {
                
                // Don't draw the bullet while it's behind the preview
                // It will continue moving in bullet_update and reappear after passing the preview
            } else {
                // Normal case - draw the full bullet
                draw_image(bullet_green, bullets[i].x, bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT, 0);
            }
        }
    }
    
    // Always redraw the plant preview on top at the end
    if (has_preview) {
        draw_plant(current_plant, get_selection_col(), get_selection_row());
    }
}

// Draw and register a peashooter plant
void Spawn_peashooter(int col, int row, unsigned long current_time_ms) {
    draw_plants_both(PLANT_TYPE_PEASHOOTER, col, row);
    if (plant_count == 0) {
        bullet_system_init(current_time_ms, 1000); // 1 seconds default
    }
    bullet_spawn_plant(col, row, current_time_ms);
}

// Save the background under a bullet
static void save_background(int x, int y, int index) {
    for (int i = 0; i < BULLET_HEIGHT; i++) {
        int bg_y = y + i;
        if (bg_y < 0 || bg_y >= GARDEN_HEIGHT) continue;
        for (int j = 0; j < BULLET_WIDTH; j++) {
            int bg_x = x + j;
            if (bg_x < 0 || bg_x >= GARDEN_WIDTH) continue;
            background_buffer[index][i * BULLET_WIDTH + j] = simulated_background[bg_y * GARDEN_WIDTH + bg_x];
        }
    }
}

// Function to draw plants on both main screen and simulated background
void draw_plants_both(int plant_type, int col, int row) {
    if (!is_valid_grid_position(col, row)) {
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
            return;
    }
    
    // Draw on both main screen and simulated background
    draw_image_both(plant_sprite, x, y, PLANT_WIDTH, PLANT_HEIGHT, 0);
}

// Function to draw image on both main screen and simulated background
void draw_image_both(const unsigned int pixel_data[], int pos_x, int pos_y, int width, int height, int show_transparent) {
    // Draw on main screen
    draw_image(pixel_data, pos_x, pos_y, width, height, show_transparent);
    
    // Draw on simulated background
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int pixel_index = y * width + x;
            int screen_x = pos_x + x;
            int screen_y = pos_y + y;
            
            // Skip transparent pixels if show_transparent is 0
            if (pixel_data[pixel_index] == 0 && !show_transparent) {
                continue;
            }
            
            // Only update if within bounds
            if (screen_x >= 0 && screen_x < GARDEN_WIDTH && 
                screen_y >= 0 && screen_y < GARDEN_HEIGHT) {
                simulated_background[screen_y * GARDEN_WIDTH + screen_x] = pixel_data[pixel_index];
            }
        }
    }
}


void check_bullet_zombie_collisions(Zombie *zombie) {
    if (!zombie->active) return;
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            // Simple bounding box collision
            if (bullets[i].x < zombie->x + ZOMBIE_WIDTH &&
                bullets[i].x + BULLET_WIDTH > zombie->x &&
                bullets[i].y < zombie->y + ZOMBIE_HEIGHT &&
                bullets[i].y + BULLET_HEIGHT > zombie->y) {
                apply_bullet_damage(&bullets[i], zombie);
            }
        }
    }
}

void apply_bullet_damage(Bullet *bullet, Zombie *zombie) {
    if (!zombie->active) return;
    int dmg = get_plant_damage(bullet->plant_type);
    zombie->health -= dmg;
    
    restore_background_area(bullet->x, bullet->y, BULLET_WIDTH, BULLET_HEIGHT, 0, 0);
    
    if (zombie->health <= 0) {
        zombie->health = 0;
        if (zombie->active) {
            zombie->active = 0;
            register_zombie_on_row(zombie->row, 0);
            restore_background_area(zombie->x, zombie->y, ZOMBIE_WIDTH, ZOMBIE_HEIGHT, 0, 0);
            uart_puts("Zombie removed\n");
        }
    }
    
    bullet->active = 0;
    uart_puts("Zombie health: ");
    uart_dec(zombie->health);
    uart_puts("\n");
}

// Restore the background under a bullet
static void restore_background(int x, int y, int index) {
    int preview_x = 0, preview_y = 0;
    int preview_width = PLANT_WIDTH, preview_height = PLANT_HEIGHT;
    int has_preview = 0;
    int current_plant = get_selection_current_plant();
    
    // Calculate preview area if a plant is selected
    if (current_plant != -1) {
        has_preview = 1;
        grid_to_pixel(get_selection_col(), get_selection_row(), &preview_x, &preview_y);
        int offset_x, offset_y;
        calculate_grid_center_offset(preview_width, preview_height, &offset_x, &offset_y);
        preview_x += offset_x;
        preview_y += offset_y;
    }
    
    // Restore background pixel by pixel, skipping the preview area
    for (int i = 0; i < BULLET_HEIGHT; i++) {
        int bg_y = y + i;
        if (bg_y < 0 || bg_y >= GARDEN_HEIGHT) continue;
        
        for (int j = 0; j < BULLET_WIDTH; j++) {
            int bg_x = x + j;
            if (bg_x < 0 || bg_x >= GARDEN_WIDTH) continue;
            
            // Skip restoration if this pixel is in the preview area
            if (has_preview &&
                bg_x >= preview_x && bg_x < preview_x + preview_width &&
                bg_y >= preview_y && bg_y < preview_y + preview_height) {
                continue;
            }
            
            // Restore this pixel from the simulated background
            if (bg_x < PHYSICAL_WIDTH && bg_y < PHYSICAL_HEIGHT) {
                draw_pixel(bg_x, bg_y, simulated_background[bg_y * GARDEN_WIDTH + bg_x]);
            }
        }
    }
}

// // Clear the previous positions of all bullets
// static void clear_bullet_area() {
//     for (int i = 0; i < MAX_BULLETS; i++) {
//         restore_background(bullets[i].prev_x, bullets[i].prev_y, i);
//     }
// }

void reset_zombie_counts(void) {
    for (int i = 0; i < GRID_ROWS; i++) {
        zombies_on_row[i] = 0;
    }
}

// --- Testing Game and Testing Functions---

// Draw and register a peashooter plant
void spawn_peashooter(int col, int row, unsigned long current_time_ms) {
    draw_plants_both(PLANT_TYPE_PEASHOOTER, col, row);
    if (plant_count == 0) {
        bullet_system_init(current_time_ms, 1000); // 1 seconds default
    }
    bullet_spawn_plant(col, row, current_time_ms);
}

void bullet_game() {
    // --- Initialization Block ---
    // Initialize the framebuffer and draw the garden background
    framebf_init();
    for (int i = 0; i < GARDEN_WIDTH * GARDEN_HEIGHT; i++) {
        simulated_background[i] = GARDEN[i];
    }
    draw_image_both(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    draw_grid();

    // --- Timer Setup Block ---
    // Set up timers for game loop and zombie updates
    unsigned long freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    unsigned long start_counter;
    asm volatile("mrs %0, cntpct_el0" : "=r"(start_counter));
    unsigned long start_ms = start_counter * 1000 / freq;

    // --- Plant Spawning Block ---
    // Spawn peashooter plants in rows 0-4 and an extra one in row 0
    for (int row = 0; row < 5; row++) {
        spawn_peashooter(1, row, start_ms);
    }
    spawn_peashooter(2, 0, start_ms); 

    // --- Zombie Spawning Block ---
    // Spawn a test zombie in row 0
    Zombie test_zombie = spawn_zombie(1, 0);
    const unsigned int zombie_frame_interval = 200; // 5 FPS for zombies
    unsigned long last_zombie_frame_time = start_ms;
    const unsigned int FRAME_INTERVAL_MS = 30; // ~33 FPS

    // --- Main Game Loop Block ---
    // Run the game loop until game_over is set
    while (!game_over) {
        // Wait for the next frame
        set_wait_timer(1, FRAME_INTERVAL_MS);

        // Check for quit command
        char c = getUart();
        if (c == 'q') {
            game_over = 1;
        }

    // Update current time
    unsigned long current_counter;
    asm volatile("mrs %0, cntpct_el0" : "=r"(current_counter));
    unsigned long current_time_ms = current_counter * 1000 / freq;

    if ((current_time_ms - last_zombie_frame_time) >= zombie_frame_interval) {
        update_zombie_position(&test_zombie);
        last_zombie_frame_time = current_time_ms;
    }

    // Always update and draw bullets, even if the zombie is dead
    bullet_update(current_time_ms);
    bullet_draw();

    // Only check for collisions if zombie is alive
    if (test_zombie.active) {
        check_bullet_zombie_collisions(&test_zombie);
    }
        // Wait for the frame to end
        set_wait_timer(0, 0);
    }

    // --- Game Over Block ---
    uart_puts("Game over\n");
}