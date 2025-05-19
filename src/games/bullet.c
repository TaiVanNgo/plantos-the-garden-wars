#include "../include/framebf.h"
#include "../include/uart0.h"
#include "bullet.h"
#include "../assets/backgrounds/background.h"

extern unsigned char *fb; 

struct bullets bullets;

// Box dimensions
#define BOX_SIZE 20
#define DEFAULT_BULLET_SPEED 1
#define DEFAULT_COLLISION_DELAY 10 


#define UI_X 10
#define UI_Y 10
#define UI_W 350
#define UI_H 40

// Buffer to store the background under the bullet
static unsigned int background_buffer[BOX_SIZE * BOX_SIZE];

#define GRID_COLS 8
#define GRID_ROWS 4
#define CELL_WIDTH  (800 / 8)
#define CELL_HEIGHT (600 / 4)

void handle_background(int x, int y, int restore) {
    for (int i = 0; i < BOX_SIZE; i++) {
        int bg_y = y + i;
        if (bg_y < 0 || bg_y >= BACKGROUND_HEIGHT) continue;
        for (int j = 0; j < BOX_SIZE; j++) {
            int bg_x = x + j;
            if (bg_x < 0 || bg_x >= BACKGROUND_WIDTH) continue;
            if (restore) {
                if (bg_x < PHYSICAL_WIDTH && bg_y < PHYSICAL_HEIGHT)
                    draw_pixel(bg_x, bg_y, background_buffer[i * BOX_SIZE + j]);
            } else {
                background_buffer[i * BOX_SIZE + j] = GAME_BACKGROUND[bg_y * BACKGROUND_WIDTH + bg_x];
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
    draw_rect(UI_X, UI_Y, UI_X + UI_W, UI_Y + UI_H, BLACK, 1);
}

static void init_game() {
 
    bullets.bullet_x = 50;
    bullets.bullet_y = PHYSICAL_HEIGHT / 2;
    bullets.prev_bullet_x = bullets.bullet_x;
    bullets.prev_bullet_y = bullets.bullet_y;
    

    bullets.target_x = PHYSICAL_WIDTH - 100;
    bullets.target_y = PHYSICAL_HEIGHT / 2;
    
   
    bullets.bullet_speed = DEFAULT_BULLET_SPEED;
    bullets.last_bullet_speed = DEFAULT_BULLET_SPEED;
    bullets.collision_timer = 0;
    bullets.collision_delay = DEFAULT_COLLISION_DELAY;
    bullets.bullet_active = 1;  
    
    bullets.score = 0;
    bullets.last_score = -1;
    bullets.game_over = 0;
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
    if (bullets.bullet_active) {
        // save_background(bullets.bullet_x, bullets.bullet_y);
        draw_rect(bullets.bullet_x, bullets.bullet_y, 
                  bullets.bullet_x + BOX_SIZE, bullets.bullet_y + BOX_SIZE, 
                  BLUE, 1);
    }
    
    // Draw target 
    draw_rect(bullets.target_x, bullets.target_y,
              bullets.target_x + BOX_SIZE, bullets.target_y + BOX_SIZE,
              RED, 1);
    

    if (bullets.score != bullets.last_score || bullets.bullet_speed != bullets.last_bullet_speed) {
        clear_score_area();
        char info_str[64];
        format_score_speed(info_str, bullets.score, bullets.bullet_speed);
        draw_string(UI_X, UI_Y, info_str, WHITE, 2);
        bullets.last_score = bullets.score;
        bullets.last_bullet_speed = bullets.bullet_speed;
    }
}

static int check_collision() {
    if (!bullets.bullet_active) {
        return 0;
    }
    
    // Check if bullet box overlaps with target box
    if (bullets.bullet_x < bullets.target_x + BOX_SIZE &&
        bullets.bullet_x + BOX_SIZE > bullets.target_x &&
        bullets.bullet_y < bullets.target_y + BOX_SIZE &&
        bullets.bullet_y + BOX_SIZE > bullets.target_y) {
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

void bullet_set_position(int x, int y) {
    bullets.bullet_x = x;
    bullets.bullet_y = y;
    bullets.prev_bullet_x = x;
    bullets.prev_bullet_y = y;
    bullets.bullet_active = 1;
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

void bullet_game() {
    framebf_init();
    bullets.bullet_x = 50;
    bullets.bullet_y = PHYSICAL_HEIGHT / 2;
    bullets.prev_bullet_x = bullets.bullet_x;
    bullets.prev_bullet_y = bullets.bullet_y;
    bullets.target_x = PHYSICAL_WIDTH - 100;
    bullets.target_y = PHYSICAL_HEIGHT / 2;
    bullets.bullet_speed = 2;
    bullets.bullet_active = 0;
    bullets.score = 0;
    bullets.game_over = 0;
    clear_screen();
    draw_image(GAME_BACKGROUND, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
    draw_grid();
    // draw_sunflower(100, 200);
    // draw_peashooter(200, 200);
    bullet_set_grid(1, 3); 
    update_bullets();
    while (!bullets.game_over) {
        char c = getUart();
        if (c == ' ') {
            if (!bullets.bullet_active) {
                bullet_set_grid(0, 4);
            }
        } else if (c == 'q') {
            bullets.game_over = 1;
        }
        clear_bullet_area();
        draw_grid();
        // draw_sunflower(100, 200);
        // draw_peashooter(200, 200);
        update_bullets();
        if (bullets.bullet_active) {
            save_background(bullets.bullet_x, bullets.bullet_y);
            for (int i = 0; i < BOX_SIZE; i++) {
                for (int j = 0; j < BOX_SIZE; j++) {
                    int px = bullets.bullet_x + j;
                    int py = bullets.bullet_y + i;
                    if (px < 0 || px >= PHYSICAL_WIDTH || py < 0 || py >= PHYSICAL_HEIGHT)
                        continue;
                    draw_pixel(px, py, BLUE);
                }
            }
        }
        draw_rect(bullets.target_x, bullets.target_y,
                 bullets.target_x + BOX_SIZE, bullets.target_y + BOX_SIZE,
                 RED, 1);
        char score_str[32];
        int i = 0;
        score_str[i++] = 'S';
        score_str[i++] = 'c';
        score_str[i++] = 'o';
        score_str[i++] = 'r';
        score_str[i++] = 'e';
        score_str[i++] = ':';
        score_str[i++] = ' ';
        int score = bullets.score;
        if (score == 0) {
            score_str[i++] = '0';
        } else {
            while (score > 0) {
                score_str[i++] = '0' + (score % 10);
                score /= 10;
            }
        }
        score_str[i] = '\0';
        draw_string(10, 10, score_str, WHITE, 2);
        for (volatile int i = 0; i < 400000; i++);
    }
    
    uart_puts("Game over\n");
}
