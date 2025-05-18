#include "../include/framebf.h"
#include "../include/uart0.h"
#include "bullet.h"

struct bullets bullets;

// Box dimensions
#define BOX_SIZE 20
#define DEFAULT_BULLET_SPEED 5
#define DEFAULT_COLLISION_DELAY 10 


#define UI_X 10
#define UI_Y 10
#define UI_W 350
#define UI_H 40

static void clear_bullet_area() {
    draw_rect(bullets.prev_bullet_x, bullets.prev_bullet_y,
              bullets.prev_bullet_x + BOX_SIZE, bullets.prev_bullet_y + BOX_SIZE,
              BLACK, 1);
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

static void draw_game() {

    clear_bullet_area();
    if (bullets.bullet_active) {
        draw_rect(bullets.bullet_x, bullets.bullet_y, 
                  bullets.bullet_x + BOX_SIZE, bullets.bullet_y + BOX_SIZE, 
                  BLUE, 1);
    }
    
    // Draw target (red box)
    draw_rect(bullets.target_x, bullets.target_y,
              bullets.target_x + BOX_SIZE, bullets.target_y + BOX_SIZE,
              RED, 1);
    

    if (bullets.score != bullets.last_score || bullets.bullet_speed != bullets.last_bullet_speed) {
        clear_score_area();
        char info_str[64];
        sprintf(info_str, "Score: %d | Speed: %d", bullets.score, bullets.bullet_speed);
        draw_string(UI_X, UI_Y, info_str, WHITE, 2);
        bullets.last_score = bullets.score;
        bullets.last_bullet_speed = bullets.bullet_speed;
    }
}

static int check_collision() {
    // Only check collision if bullet is active
    if (!bullets.bullet_active) return 0;
    
    // Check if bullet box overlaps with target box
    if (bullets.bullet_x < bullets.target_x + BOX_SIZE &&
        bullets.bullet_x + BOX_SIZE > bullets.target_x &&
        bullets.bullet_y < bullets.target_y + BOX_SIZE &&
        bullets.bullet_y + BOX_SIZE > bullets.target_y) {
        return 1;
    }
    return 0;
}

static void update_bullets() {
    // Only move bullet if it's active
    if (bullets.bullet_active) {
        // Save current position as previous
        bullets.prev_bullet_x = bullets.bullet_x;
        bullets.prev_bullet_y = bullets.bullet_y;
        // Move bullet right
        bullets.bullet_x += bullets.bullet_speed;
        // Update collision timer
        bullets.collision_timer++;
        // Only check collision when timer is up
        if (bullets.collision_timer >= bullets.collision_delay) {
            bullets.collision_timer = 0;  // Reset timer
            // Check if bullet hit target
            if (check_collision()) {
                bullets.score++;
                bullets.bullet_active = 0;  // Deactivate bullet
            }
        }
        // Check if bullet went off screen
        if (bullets.bullet_x > PHYSICAL_WIDTH) {
            bullets.bullet_active = 0;  // Deactivate bullet
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
