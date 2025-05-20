#include "../include/framebf.h"
#include "../include/uart0.h"
#include "../include/plants.h"
#include "bullet.h"
#include "../assets/backgrounds/garden.h"
#include "../assets/sprites/plants/plants_sprites.h"

extern unsigned char *fb; 

struct bullets bullets;

// Simple random number generator
static unsigned int next = 1;
static int rand(void) {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

// Buffer to store the background under the bullets
static unsigned int background_buffer[5][BULLET_WIDTH * BULLET_HEIGHT];

// Simulated background buffer
static unsigned int sim_bg[GARDEN_WIDTH * GARDEN_HEIGHT];

// Function to draw plants both on screen and in simulated background
void draw_plants_both(int plant_type, int col, int row) {
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
            return;
    }
    
    // Draw on screen
    draw_image(plant_sprite, x, y, PLANT_WIDTH, PLANT_HEIGHT, 0);
    
    // Draw in simulated background
    draw_on_simulated_background(sim_bg, plant_sprite, x, y, PLANT_WIDTH, PLANT_HEIGHT, GARDEN_WIDTH);
}

// Function to draw image both on screen and in simulated background
void draw_image_both(const unsigned int pixel_data[], int pos_x, int pos_y, int width, int height, int show_transparent) {
    // Draw on screen
    draw_image(pixel_data, pos_x, pos_y, width, height, show_transparent);
    
    // Draw in simulated background
    draw_on_simulated_background(sim_bg, pixel_data, pos_x, pos_y, width, height, GARDEN_WIDTH);
}

void handle_background(int x, int y, int restore, int bullet_index) {
    for (int i = 0; i < BULLET_HEIGHT; i++) {
        int bg_y = y + i;
        if (bg_y < 0 || bg_y >= GARDEN_HEIGHT) continue;
        for (int j = 0; j < BULLET_WIDTH; j++) {
            int bg_x = x + j;
            if (bg_x < 0 || bg_x >= GARDEN_WIDTH) continue;
            if (restore) {
                if (bg_x < PHYSICAL_WIDTH && bg_y < PHYSICAL_HEIGHT)
                    draw_pixel(bg_x, bg_y, background_buffer[bullet_index][i * BULLET_WIDTH + j]);
            } else {
                background_buffer[bullet_index][i * BULLET_WIDTH + j] = GARDEN[bg_y * GARDEN_WIDTH + bg_x];
            }
        }
    }
}

#define save_background(x, y, index) handle_background((x), (y), 0, (index))
#define restore_background(x, y, index) handle_background((x), (y), 1, (index))

static void clear_bullet_area() {
    for (int i = 0; i < 5; i++) {
        if (bullets.bullet_active[i]) {
            restore_background(bullets.bullet_x[i], bullets.bullet_y[i], i);
        }
    }
}

static void clear_score_area() {
    draw_rect(10, 10, 10 + 150, 10 + 40, BLACK, 1);
}

static void init_game() {
    // Initialize all bullets
    for (int i = 0; i < 5; i++) {
        // Calculate position for each row
        int shooter_x = PLANT_GRID_LEFT_MARGIN + ((PLANT_COL_WIDTH - PLANT_WIDTH) / 2);
        int shooter_y = PLANT_GRID_TOP_MARGIN + (i * PLANT_ROW_HEIGHT) + 
                      ((PLANT_ROW_HEIGHT - PLANT_HEIGHT) / 2);
        
        bullets.bullet_x[i] = shooter_x + PLANT_WIDTH;
        bullets.bullet_y[i] = shooter_y + (PLANT_HEIGHT / 2) - (BULLET_HEIGHT / 2);
        bullets.prev_bullet_x[i] = bullets.bullet_x[i];
        bullets.prev_bullet_y[i] = bullets.bullet_y[i];
        bullets.bullet_active[i] = 0;
    }
    
    // Target position on the right side of the screen
    bullets.target_x = PHYSICAL_WIDTH - 100;
    bullets.target_y = PLANT_GRID_TOP_MARGIN + PLANT_ROW_HEIGHT;
    
    bullets.bullet_speed = 3;
    bullets.score = 0;
    bullets.last_score = -1;
    bullets.game_over = 0;
}

static void format_score(char *buf, int score) {
    int i = 0;

    const char score_prefix[] = "Score: ";
    while (score_prefix[i] != '\0') {
        buf[i] = score_prefix[i];
        i++;
    }
    
    int score_copy = score;
    if (score_copy == 0) {
        buf[i++] = '0';
    } else {
        char digits[10]; 
        int digit_count = 0;
        
        while (score_copy > 0) {
            digits[digit_count++] = '0' + (score_copy % 10);
            score_copy /= 10;
        }
        
        // Add digits in correct order
        for (int j = digit_count - 1; j >= 0; j--) {
            buf[i++] = digits[j];
        }
    }
    
    buf[i] = '\0';
}

static int check_collision(int bullet_index) {
    if (!bullets.bullet_active[bullet_index]) {
        return 0;
    }
    
    int target_width = 40;
    int target_height = 70;
    
    if (bullets.bullet_x[bullet_index] < bullets.target_x + target_width &&
        bullets.bullet_x[bullet_index] + BULLET_WIDTH > bullets.target_x &&
        bullets.bullet_y[bullet_index] < bullets.target_y + target_height &&
        bullets.bullet_y[bullet_index] + BULLET_HEIGHT > bullets.target_y) {
        return 1;
    }
    return 0;
}

void update_bullets() {
    for (int i = 0; i < 5; i++) {
        if (bullets.bullet_active[i]) {
            // Save current position as previous
            bullets.prev_bullet_x[i] = bullets.bullet_x[i];
            bullets.prev_bullet_y[i] = bullets.bullet_y[i];
            
            // Move bullet right
            bullets.bullet_x[i] += bullets.bullet_speed;
            
            // Check collision
            if (check_collision(i)) {
                bullets.score++;
                bullets.bullet_active[i] = 0;
            }
            if (bullets.bullet_x[i] > PHYSICAL_WIDTH) {
                bullets.bullet_active[i] = 0;
            }
        }
    }
}

void fire_bullet() {
    // Fire all bullets
    for (int i = 0; i < 5; i++) {
        if (!bullets.bullet_active[i]) {
            // Calculate peashooter position for this row
            int shooter_x = PLANT_GRID_LEFT_MARGIN + ((PLANT_COL_WIDTH - PLANT_WIDTH) / 2);
            int shooter_y = PLANT_GRID_TOP_MARGIN + (i * PLANT_ROW_HEIGHT) + 
                          ((PLANT_ROW_HEIGHT - PLANT_HEIGHT) / 2);
            
            bullets.bullet_x[i] = shooter_x + PLANT_WIDTH;
            bullets.bullet_y[i] = shooter_y + (PLANT_HEIGHT / 2) - (BULLET_HEIGHT / 2);
            bullets.prev_bullet_x[i] = bullets.bullet_x[i];
            bullets.prev_bullet_y[i] = bullets.bullet_y[i];
            bullets.bullet_active[i] = 1;
            
            save_background(bullets.bullet_x[i], bullets.bullet_y[i], i);
        }
    }
}

void bullet_game() {
    framebf_init();
    
    // Initialize game state
    init_game();
    
    // Initialize the simulated background with the garden image
    for (int i = 0; i < GARDEN_WIDTH * GARDEN_HEIGHT; i++) {
        sim_bg[i] = GARDEN[i];
    }
    
    // Draw the garden background
    draw_image_both(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    
    // Draw the plant grid
    draw_plant_grid();
    
    // Place peashooters in each row
    for (int row = 0; row < 5; row++) {
        draw_plants_both(PLANT_TYPE_PEASHOOTER, 1, row);
    }
    
    while (!bullets.game_over) {
        char c = getUart();
        if (c == ' ') {
            fire_bullet();
        } else if (c == 'q') {
            bullets.game_over = 1;
        }
        
        clear_bullet_area();
        update_bullets();
        
        // Draw all active bullets
        for (int i = 0; i < 5; i++) {
            if (bullets.bullet_active[i]) {
                save_background(bullets.bullet_x[i], bullets.bullet_y[i], i);
                draw_image_both(bullet_green, bullets.bullet_x[i], bullets.bullet_y[i], BULLET_WIDTH, BULLET_HEIGHT, 0);
            }
        }
        
        // Draw target (simple red rectangle)
        draw_rect(bullets.target_x, bullets.target_y, 
                 bullets.target_x + 40, bullets.target_y + 70,
                 RED, 1);
        
        // Display score
        char score_str[32];
        format_score(score_str, bullets.score);
        clear_score_area();
        draw_string(10, 10, score_str, WHITE, 2);
        
        // Small delay to control game speed
        for (volatile int i = 0; i < 400000; i++);
    }
    
    uart_puts("Game over\n");
}