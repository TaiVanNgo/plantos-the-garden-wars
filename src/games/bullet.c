#include "../include/bullet.h"
extern unsigned char *fb; 

#define MAX_BULLETS 25 // 5 rows * 5 bullets per row

typedef struct {
    int x, y;
    int prev_x, prev_y;
    int row;
    int active;
} Bullet;

static Bullet bullets[MAX_BULLETS];
static unsigned long last_fire_time[5] = {0, 0, 0, 0, 0};
static const unsigned int fire_interval_ms = 2000; // 2 seconds
// Add static game state variables
static int target_x;
static int target_y;
static int bullet_speed;
static int score;
static int last_score;
static int game_over;

// Simple random number generator
static unsigned int next = 1;
static int rand(void) {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

// Buffer to store the background under the bullets
static unsigned int background_buffer[MAX_BULLETS][BULLET_WIDTH * BULLET_HEIGHT];

// Simulated background buffer
static unsigned int sim_bg[GARDEN_WIDTH * GARDEN_HEIGHT];

// Place peashooters and initialize their bullet positions and states
void place_plants() {
    for (int row = 0; row < 5; row++) {
        draw_plants_both(PLANT_TYPE_PEASHOOTER, 1, row);
    }
}

// Function to draw plants both on screen and in simulated background
void draw_plants_both(int plant_type, int col, int row) {
    // Check if the column and row are within valid bounds
    if (col < 0 || col >= GRID_COLS || row < 0 || row >= GRID_ROWS) {
        return; // Invalid position, do nothing
    }
    
    // Calculate pixel coordinates from grid position, centered in cell
    int horizontal_offset = (GRID_COL_WIDTH - PLANT_WIDTH) / 2;
    int vertical_offset = (GRID_ROW_HEIGHT - PLANT_HEIGHT) / 2;
    int x = GRID_LEFT_MARGIN + (col * GRID_COL_WIDTH) + horizontal_offset;
    int y = GRID_TOP_MARGIN + (row * GRID_ROW_HEIGHT) + vertical_offset;
    
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
    draw_on_simulated_background(sim_bg, pixel_data, pos_x, pos_y, width,  GARDEN_WIDTH, GARDEN_WIDTH);
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
                background_buffer[bullet_index][i * BULLET_WIDTH + j] = sim_bg[bg_y * GARDEN_WIDTH + bg_x];
            }
        }
    }
}

#define save_background(x, y, index) handle_background((x), (y), 0, (index))
#define restore_background(x, y, index) handle_background((x), (y), 1, (index))

static void clear_bullet_area() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            restore_background(bullets[i].prev_x, bullets[i].prev_y, i);
        }
    }
}

static void clear_score_area() {
    draw_rect(10, 10, 10 + 150, 10 + 40, BLACK, 1);
}

static void init_game() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active = 0;
    }
    // Target position on the right side of the screen
    target_x = PHYSICAL_WIDTH - 100;
    target_y = GRID_TOP_MARGIN + GRID_ROW_HEIGHT;
    bullet_speed = 3;
    score = 0;
    last_score = -1;
    game_over = 0;
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

void fire_bullet_for_row(int row) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) {
            int shooter_x = GRID_LEFT_MARGIN + ((GRID_COL_WIDTH - PLANT_WIDTH) / 2);
            int shooter_y = GRID_TOP_MARGIN + (row * GRID_ROW_HEIGHT) +
                            ((GRID_ROW_HEIGHT - PLANT_HEIGHT) / 2);
            bullets[i].x = shooter_x + PLANT_WIDTH;
            bullets[i].y = shooter_y + (PLANT_HEIGHT / 2) - (BULLET_HEIGHT / 2);
            bullets[i].prev_x = bullets[i].x;
            bullets[i].prev_y = bullets[i].y;
            bullets[i].row = row;
            bullets[i].active = 1;
            save_background(bullets[i].x, bullets[i].y, i);
            break;
        }
    }
}

void update_bullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].prev_x = bullets[i].x;
            bullets[i].prev_y = bullets[i].y;
            bullets[i].x += bullet_speed;
            // Check collision
            int target_width = 40;
            int target_height = 70;
            if (bullets[i].x < target_x + target_width &&
                bullets[i].x + BULLET_WIDTH > target_x &&
                bullets[i].y < target_y + target_height &&
                bullets[i].y + BULLET_HEIGHT > target_y) {
                bullets[i].active = 0;
                score++;
            }
            if (bullets[i].x > PHYSICAL_WIDTH) {
                bullets[i].active = 0;
            }
        }
    }
}

void bullet_game() {
    framebf_init();
    init_game();
    for (int i = 0; i < GARDEN_WIDTH * GARDEN_HEIGHT; i++) {
        sim_bg[i] = GARDEN[i];
    }
    draw_image_both(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    draw_grid();
    place_plants();
    set_wait_timer(1, 16);
    unsigned long freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    unsigned long start_counter;
    asm volatile("mrs %0, cntpct_el0" : "=r"(start_counter));
    unsigned long start_ms = start_counter * 1000 / freq;
    for (int i = 0; i < 5; i++) {
        last_fire_time[i] = start_ms - (i * 400); // Staggered start
    }
    while (!game_over) {
        char c = getUart();
        if (c == 'q') {
            game_over = 1;
        }
        unsigned long current_counter;
        asm volatile("mrs %0, cntpct_el0" : "=r"(current_counter));
        unsigned long current_time_ms = current_counter * 1000 / freq;
        for (int row = 0; row < 5; row++) {
            if ((current_time_ms - last_fire_time[row]) >= fire_interval_ms) {
                fire_bullet_for_row(row);
                last_fire_time[row] = current_time_ms;
            }
        }
        clear_bullet_area();
        update_bullets();
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].active) {
                save_background(bullets[i].x, bullets[i].y, i);
                draw_image(bullet_green, bullets[i].x, bullets[i].y, BULLET_WIDTH, BULLET_HEIGHT, 0);
            }
        }
        draw_rect(target_x, target_y, target_x + 40, target_y + 70, RED, 1);
        char score_str[32];
        format_score(score_str, score);
        clear_score_area();
        draw_string(10, 10, score_str, WHITE, 2);
        set_wait_timer(0, 0);
        set_wait_timer(1, 16);
    }
    uart_puts("Game over\n");
}