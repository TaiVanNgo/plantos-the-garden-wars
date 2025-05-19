#include "../include/framebf.h"
#include "../include/uart0.h"
#include "../include/plants.h"
#include "bullet.h"
#include "../assets/backgrounds/garden.h"

extern unsigned char *fb; 

struct bullets bullets;

// Buffer to store the background under the bullet
static unsigned int background_buffer[BULLET_WIDTH * BULLET_HEIGHT];

void handle_background(int x, int y, int restore) {
    for (int i = 0; i < BULLET_HEIGHT; i++) {
        int bg_y = y + i;
        if (bg_y < 0 || bg_y >= BACKGROUND_HEIGHT) continue;
        for (int j = 0; j < BULLET_WIDTH; j++) {
            int bg_x = x + j;
            if (bg_x < 0 || bg_x >= BACKGROUND_WIDTH) continue;
            if (restore) {
                if (bg_x < PHYSICAL_WIDTH && bg_y < PHYSICAL_HEIGHT)
                    draw_pixel(bg_x, bg_y, background_buffer[i * BULLET_WIDTH + j]);
            } else {
                background_buffer[i * BULLET_WIDTH + j] = GARDEN[bg_y * BACKGROUND_WIDTH + bg_x];
            }
        }
    }
}

#define save_background(x, y) handle_background((x), (y), 0)
#define restore_background(x, y) handle_background((x), (y), 1)

static void clear_bullet_area() {
    if (bullets.bullet_active) {
        restore_background(bullets.prev_bullet_x, bullets.prev_bullet_y);
    }
}

static void clear_score_area() {
    draw_rect(10, 10, 10 + 150, 10 + 40, BLACK, 1);
}

static void init_game() {
    // Fixed row for the peashooter (using row 1)
    int fixed_row = 1;
    
    // Position the peashooter in the first column
    int shooter_x = PLANT_GRID_LEFT_MARGIN + ((PLANT_COL_WIDTH - PLANT_WIDTH) / 2);
    int shooter_y = PLANT_GRID_TOP_MARGIN + (fixed_row * PLANT_ROW_HEIGHT) + 
                  ((PLANT_ROW_HEIGHT - PLANT_HEIGHT) / 2);
    
    // Initial bullet position (at the shooter's "mouth")
    bullets.bullet_x = shooter_x + PLANT_WIDTH;
    bullets.bullet_y = shooter_y + (PLANT_HEIGHT / 2) - (BULLET_HEIGHT / 2);
    bullets.prev_bullet_x = bullets.bullet_x;
    bullets.prev_bullet_y = bullets.bullet_y;

    // Target position on the right side of the screen
    bullets.target_x = PHYSICAL_WIDTH - 100;
    bullets.target_y = shooter_y;
    
    bullets.bullet_speed = 3;
    bullets.bullet_active = 0;  
    
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

static int check_collision() {
    if (!bullets.bullet_active) {
        return 0;
    }
    
    // Check if bullet overlaps with target
    
    int target_width = 40;
    int target_height = 70;
    
    if (bullets.bullet_x < bullets.target_x + target_width &&
        bullets.bullet_x + BULLET_WIDTH > bullets.target_x &&
        bullets.bullet_y < bullets.target_y + target_height &&
        bullets.bullet_y + BULLET_HEIGHT > bullets.target_y) {
        return 1;
    }
    return 0;
}

void update_bullets() {
    if (bullets.bullet_active) {
        // Save current position as previous
        bullets.prev_bullet_x = bullets.bullet_x;
        bullets.prev_bullet_y = bullets.bullet_y;
        
        // Move bullet right
        bullets.bullet_x += bullets.bullet_speed;
        // Check collision
        if (check_collision()) {
            bullets.score++;
            bullets.bullet_active = 0;
        }
        if (bullets.bullet_x > PHYSICAL_WIDTH) {
            bullets.bullet_active = 0;
        }
    }
}

void fire_bullet() {
    if (!bullets.bullet_active) {
        // Fixed row for the peashooter (using row 1)
        int fixed_row = 1;
        
        // Calculate peashooter position
        int shooter_x = PLANT_GRID_LEFT_MARGIN + ((PLANT_COL_WIDTH - PLANT_WIDTH) / 2);
        int shooter_y = PLANT_GRID_TOP_MARGIN + (fixed_row * PLANT_ROW_HEIGHT) + 
                      ((PLANT_ROW_HEIGHT - PLANT_HEIGHT) / 2);
        
        // Set bullet position to start at the right side of the peashooter
        bullets.bullet_x = shooter_x + PLANT_WIDTH;
        bullets.bullet_y = shooter_y + (PLANT_HEIGHT / 2) - (BULLET_HEIGHT / 2);
        bullets.prev_bullet_x = bullets.bullet_x;
        bullets.prev_bullet_y = bullets.bullet_y;
        bullets.bullet_active = 1;
        
        save_background(bullets.bullet_x, bullets.bullet_y);
    }
}

void bullet_game() {
    framebf_init();
    
    // Initialize game state
    init_game();
    
    // Draw the garden background
    draw_image(GARDEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
    
    // Draw the plant grid
    draw_plant_grid();
    
    // Draw the peashooter in a fixed row (row 1)
    draw_plant(PLANT_TYPE_PEASHOOTER, 0, 1);
    while (!bullets.game_over) {
        char c = getUart();
        if (c == ' ') {
            if (!bullets.bullet_active) {
                fire_bullet();
            }
        } else if (c == 'q') {
            bullets.game_over = 1;
        }
        clear_bullet_area();
        update_bullets();
        draw_plant(PLANT_TYPE_PEASHOOTER, 0, 1);
        // Draw bullet if active
        if (bullets.bullet_active) {
            save_background(bullets.bullet_x, bullets.bullet_y);
            draw_image(bullet_green, bullets.bullet_x, bullets.bullet_y, BULLET_WIDTH, BULLET_HEIGHT, 0);
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