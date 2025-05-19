#include "../include/framebf.h"
#include "../include/uart0.h"
#include "bullet.h"
#include "../assets/backgrounds/garden.h"
#include "../../include/plants.h"

extern unsigned char *fb; 

#define MAX_BULLETS 2
// #define MAX_BULLETS 6 // Uncomment for 6 bullets

struct bullets bullets[MAX_BULLETS];

// Box dimensions
#define BOX_SIZE 20
#define DEFAULT_BULLET_SPEED 1
#define DEFAULT_COLLISION_DELAY 10 

// Simulated background buffer
static unsigned int simulated_background[GARDEN_WIDTH * GARDEN_HEIGHT];

#define UI_X 10
#define UI_Y 10
#define UI_W 350
#define UI_H 40

// Buffer to store the background under each bullet
static unsigned int background_buffer[MAX_BULLETS][BOX_SIZE * BOX_SIZE];

#define GRID_COLS 8
#define GRID_ROWS 4
#define CELL_WIDTH  (800 / 8)
#define CELL_HEIGHT (600 / 4)

void handle_background(int x, int y, int restore, int bullet_idx) {
    for (int i = 0; i < BOX_SIZE; i++) {
        int bg_y = y + i;
        if (bg_y < 0 || bg_y >= GARDEN_HEIGHT) continue;
        for (int j = 0; j < BOX_SIZE; j++) {
            int bg_x = x + j;
            if (bg_x < 0 || bg_x >= GARDEN_WIDTH) continue;
            if (restore) {
                if (bg_x < PHYSICAL_WIDTH && bg_y < PHYSICAL_HEIGHT)
                    draw_pixel(bg_x, bg_y, background_buffer[bullet_idx][i * BOX_SIZE + j]);
            } else {
                background_buffer[bullet_idx][i * BOX_SIZE + j] = get_simulated_pixel(simulated_background, bg_x, bg_y, GARDEN_WIDTH);
            }
        }
    }
}

#define save_background(x, y, idx) handle_background((x), (y), 0, (idx))
#define restore_background(x, y, idx) handle_background((x), (y), 1, (idx))

static void clear_bullet_area() {
    if (bullets[0].bullet_active) {
        restore_background(bullets[0].prev_bullet_x, bullets[0].prev_bullet_y, 0);
    }
    if (bullets[1].bullet_active) {
        restore_background(bullets[1].prev_bullet_x, bullets[1].prev_bullet_y, 1);
    }
}

static void clear_score_area() {
    draw_rect(UI_X, UI_Y, UI_X + UI_W, UI_Y + UI_H, BLACK, 1);
}

static void init_game() {
 
    bullets[0].bullet_x = 50;
    bullets[0].bullet_y = PHYSICAL_HEIGHT / 2;
    bullets[0].prev_bullet_x = bullets[0].bullet_x;
    bullets[0].prev_bullet_y = bullets[0].bullet_y;
    

    bullets[0].target_x = PHYSICAL_WIDTH - 100;
    bullets[0].target_y = PHYSICAL_HEIGHT / 2;
    
   
    bullets[0].bullet_speed = DEFAULT_BULLET_SPEED;
    bullets[0].last_bullet_speed = DEFAULT_BULLET_SPEED;
    bullets[0].collision_timer = 0;
    bullets[0].collision_delay = DEFAULT_COLLISION_DELAY;
    bullets[0].bullet_active = 1;  
    
    bullets[0].score = 0;
    bullets[0].last_score = -1;
    bullets[0].game_over = 0;

    bullets[1].bullet_x = 50;
    bullets[1].bullet_y = PHYSICAL_HEIGHT / 2;
    bullets[1].prev_bullet_x = bullets[1].bullet_x;
    bullets[1].prev_bullet_y = bullets[1].bullet_y;
    

    bullets[1].target_x = PHYSICAL_WIDTH - 100;
    bullets[1].target_y = PHYSICAL_HEIGHT / 2;
    
   
    bullets[1].bullet_speed = DEFAULT_BULLET_SPEED;
    bullets[1].last_bullet_speed = DEFAULT_BULLET_SPEED;
    bullets[1].collision_timer = 0;
    bullets[1].collision_delay = DEFAULT_COLLISION_DELAY;
    bullets[1].bullet_active = 1;  
    
    bullets[1].score = 0;
    bullets[1].last_score = -1;
    bullets[1].game_over = 0;
}


static void format_score_speed(char *buf, int score, int speed) {
    int i = 0;

    const char score_prefix[] = "Score: ";
    while (score_prefix[i] != '\0') {
        buf[i] = score_prefix[i];
        i++;
    }
    int score_copy = score;
    int score_len = 0;
    if (score_copy == 0) {
        buf[i++] = '0';
        score_len = 1;
    } else {
        int temp = score_copy;
        while (temp > 0) {
            temp /= 10;
            score_len++;
        }
        int j = i + score_len - 1;
        while (score_copy > 0) {
            buf[j--] = '0' + (score_copy % 10);
            score_copy /= 10;
        }
        i += score_len;
    }
    // " | Speed: "
    const char speed_prefix[] = " | Speed: ";
    int j = 0;
    while (speed_prefix[j] != '\0') {
        buf[i++] = speed_prefix[j++];
    }
    int speed_copy = speed;
    int speed_len = 0;
    if (speed_copy == 0) {
        buf[i++] = '0';
        speed_len = 1;
    } else {
        int temp = speed_copy;
        while (temp > 0) {
            temp /= 10;
            speed_len++;
        }
        int j = i + speed_len - 1;
        while (speed_copy > 0) {
            buf[j--] = '0' + (speed_copy % 10);
            speed_copy /= 10;
        }
        i += speed_len;
    }
    buf[i] = '\0';
}

static void draw_game() {

    clear_bullet_area();
    if (bullets[0].bullet_active) {
        // save_background(bullets[0].bullet_x, bullets[0].bullet_y, 0);
        draw_rect(bullets[0].bullet_x, bullets[0].bullet_y, 
                  bullets[0].bullet_x + BOX_SIZE, bullets[0].bullet_y + BOX_SIZE, 
                  BLUE, 1);
    }
    if (bullets[1].bullet_active) {
        // save_background(bullets[1].bullet_x, bullets[1].bullet_y, 1);
        draw_rect(bullets[1].bullet_x, bullets[1].bullet_y, 
                  bullets[1].bullet_x + BOX_SIZE, bullets[1].bullet_y + BOX_SIZE, 
                  BLUE, 1);
    }
    
    // Draw target 
    draw_rect(bullets[0].target_x, bullets[0].target_y,
              bullets[0].target_x + BOX_SIZE, bullets[0].target_y + BOX_SIZE,
              RED, 1);
    draw_rect(bullets[1].target_x, bullets[1].target_y,
              bullets[1].target_x + BOX_SIZE, bullets[1].target_y + BOX_SIZE,
              RED, 1);
    

    if (bullets[0].score != bullets[0].last_score || bullets[0].bullet_speed != bullets[0].last_bullet_speed) {
        clear_score_area();
        char info_str[64];
        format_score_speed(info_str, bullets[0].score, bullets[0].bullet_speed);
        draw_string(UI_X, UI_Y, info_str, WHITE, 2);
        bullets[0].last_score = bullets[0].score;
        // bullets[0].i = bullets[0].bullet_speed;
    }
    if (bullets[1].score != bullets[1].last_score || bullets[1].bullet_speed != bullets[1].last_bullet_speed) {
        clear_score_area();
        char info_str[64];
        format_score_speed(info_str, bullets[1].score, bullets[1].bullet_speed);
        draw_string(UI_X, UI_Y, info_str, WHITE, 2);
        bullets[1].last_score = bullets[1].score;
        bullets[1].last_bullet_speed = bullets[1].bullet_speed;
    }
}

static int check_collision(struct bullets *bullet) {
    if (!bullet->bullet_active) {
        return 0;
    }
    
    // Check if bullet box overlaps with target box
    if (bullet->bullet_x < bullet->target_x + BOX_SIZE &&
        bullet->bullet_x + BOX_SIZE > bullet->target_x &&
        bullet->bullet_y < bullet->target_y + BOX_SIZE &&
        bullet->bullet_y + BOX_SIZE > bullet->target_y) {
        return 1;
    }
    return 0;
}

void update_bullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].bullet_active) {
            handle_background(bullets[i].prev_bullet_x, bullets[i].prev_bullet_y, 1, i); // Restore
            bullets[i].prev_bullet_x = bullets[i].bullet_x;
            bullets[i].prev_bullet_y = bullets[i].bullet_y;
            bullets[i].bullet_x += bullets[i].bullet_speed;
            // Check collision
            if (check_collision(&bullets[i])) {
                bullets[i].score++;
                bullets[i].bullet_active = 0;
            }
            off_screen(&bullets[i]);
        }
    }
}

void bullet_set_position(int x, int y) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].bullet_x = x;
        bullets[i].bullet_y = y;
        bullets[i].prev_bullet_x = x;
        bullets[i].prev_bullet_y = y;
        bullets[i].bullet_active = 1;
    }
}

void check_grid_position() {

}

void draw_grid() {
    // Draw vertical lines
    for (int col = 1; col < GRID_COLS; col++) {
        int x = col * CELL_WIDTH;
        draw_rect(x, 0, x+1, 600, WHITE, 1);
    }
    // Draw horizontal lines
    for (int row = 1; row < GRID_ROWS; row++) {
        int y = row * CELL_HEIGHT;
        draw_rect(0, y, 800, y+1, WHITE, 1);
    }
}

void bullet_set_grid(int row, int col) {
    int x = col * CELL_WIDTH + (CELL_WIDTH - BOX_SIZE) / 2;
    int y = row * CELL_HEIGHT + (CELL_HEIGHT - BOX_SIZE) / 2;
    bullet_set_position(x, y);
}

void fire_bullet(int x, int y) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].bullet_active) {
            bullets[i].bullet_x = x;
            bullets[i].bullet_y = y;
            bullets[i].prev_bullet_x = x;
            bullets[i].prev_bullet_y = y;
            bullets[i].bullet_active = 1;
            handle_background(x, y, 0, i); // Save background
            break;
        }
    }
}

void update_all_bullets(void) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].bullet_active) {
            handle_background(bullets[i].prev_bullet_x, bullets[i].prev_bullet_y, 1, i); // Restore
            bullets[i].prev_bullet_x = bullets[i].bullet_x;
            bullets[i].prev_bullet_y = bullets[i].bullet_y;
            bullets[i].bullet_x += bullets[i].bullet_speed;
            handle_background(bullets[i].bullet_x, bullets[i].bullet_y, 0, i); // Save
            if (bullets[i].bullet_x > PHYSICAL_WIDTH) {
                bullets[i].bullet_active = 0;
            }
        }
    }
}

void draw_all_bullets(void) {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i].bullet_active) {
            draw_image(bullet_green, bullets[i].bullet_x, bullets[i].bullet_y, BULLET_WIDTH, BULLET_HEIGHT, 0);
        }
    }
}

// Function to draw plant both on screen and in simulated background
void draw_plant_both(int plant_type, int col, int row, const unsigned int *plant_sprite) {
    // Draw plant on screen
    draw_plant(plant_type, col, row);
    
    // Add plant to simulated background
    draw_on_simulated_background(simulated_background, plant_sprite,
        PLANT_GRID_LEFT_MARGIN + (col * PLANT_COL_WIDTH),
        PLANT_GRID_TOP_MARGIN + (row * PLANT_ROW_HEIGHT),
        PLANT_WIDTH, PLANT_HEIGHT, GARDEN_WIDTH);
}

void bullet_game() {
    framebf_init();
    clear_screen();
    
    // Create simulated background with garden
    create_simulated_background(simulated_background, GARDEN, GARDEN_WIDTH, GARDEN_HEIGHT);
    
    // Draw garden background
    draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    
    // Draw plant grid
    draw_plant_grid();
    
    // Draw plants using the new combined function
    draw_plant_both(PLANT_TYPE_SUNFLOWER, 1, 2, sunflower);
    draw_plant_both(PLANT_TYPE_PEASHOOTER, 2, 1, peashooter);
    draw_plant_both(PLANT_TYPE_SUNFLOWER, 2, 2, sunflower);
    for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].bullet_active = 0;
        bullets[i].bullet_speed = 1;
    }
    
    while (1) {
        char c = getUart();
        if (c == ' ') {
            // Fire bullets from both plants
            fire_bullet(PLANT_GRID_LEFT_MARGIN + (1 * PLANT_COL_WIDTH), PLANT_GRID_TOP_MARGIN + (1 * PLANT_ROW_HEIGHT));
            fire_bullet(PLANT_GRID_LEFT_MARGIN + (2 * PLANT_COL_WIDTH), PLANT_GRID_TOP_MARGIN + (2 * PLANT_ROW_HEIGHT));
        } else if (c == 'q') {
            break;
        }
        
        // Update and draw bullets
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i].bullet_active) {
                // 1. Restore background at previous position
                restore_background(bullets[i].prev_bullet_x, bullets[i].prev_bullet_y, i);
                // 2. Update previous position to current
                bullets[i].prev_bullet_x = bullets[i].bullet_x;
                bullets[i].prev_bullet_y = bullets[i].bullet_y;
                // 3. Update bullet position
                bullets[i].bullet_x += bullets[i].bullet_speed;
                // 4. Save background at new position
                save_background(bullets[i].bullet_x, bullets[i].bullet_y, i);
                // 5. Draw bullet at new position
                draw_image(bullet_green, bullets[i].bullet_x, bullets[i].bullet_y, BULLET_WIDTH, BULLET_HEIGHT, 0);
                if (bullets[i].bullet_x > PHYSICAL_WIDTH) {
                    bullets[i].bullet_active = 0;
                }
            }
        }
        // Add a smaller delay to control bullet speed
        for (volatile int i = 0; i < 100000; i++);
    }
}

void off_screen(struct bullets *bullet) {
    if (bullet->bullet_x > PHYSICAL_WIDTH) {
        handle_background(bullet->prev_bullet_x, bullet->prev_bullet_y, 1, 0);
        bullet->bullet_active = 0;
    }
}
