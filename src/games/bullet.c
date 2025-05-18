#include "../include/framebf.h"
#include "../include/uart0.h"
#include "bullet.h"

// Game state
struct {
    int bullet_x;
    int bullet_y;
    int prev_bullet_x;
    int prev_bullet_y;
    int target_x;
    int target_y;
    int score;
    int last_score;
    int game_over;
    int bullet_speed;    
    int last_bullet_speed;
    int collision_timer; // Timer 
    int collision_delay;  
    int bullet_active;   
} game_state;

// Box dimensions
#define BOX_SIZE 20
#define DEFAULT_BULLET_SPEED 5
#define DEFAULT_COLLISION_DELAY 10 


#define UI_X 10
#define UI_Y 10
#define UI_W 350
#define UI_H 40

static void clear_bullet_area() {
    draw_rect(game_state.prev_bullet_x, game_state.prev_bullet_y,
              game_state.prev_bullet_x + BOX_SIZE, game_state.prev_bullet_y + BOX_SIZE,
              BLACK, 1);
}

static void clear_score_area() {
    draw_rect(UI_X, UI_Y, UI_X + UI_W, UI_Y + UI_H, BLACK, 1);
}

static void init_game() {
 
    game_state.bullet_x = 50;
    game_state.bullet_y = PHYSICAL_HEIGHT / 2;
    game_state.prev_bullet_x = game_state.bullet_x;
    game_state.prev_bullet_y = game_state.bullet_y;
    

    game_state.target_x = PHYSICAL_WIDTH - 100;
    game_state.target_y = PHYSICAL_HEIGHT / 2;
    
   
    game_state.bullet_speed = DEFAULT_BULLET_SPEED;
    game_state.last_bullet_speed = DEFAULT_BULLET_SPEED;
    game_state.collision_timer = 0;
    game_state.collision_delay = DEFAULT_COLLISION_DELAY;
    game_state.bullet_active = 1;  
    
    game_state.score = 0;
    game_state.last_score = -1;
    game_state.game_over = 0;
}

static void draw_game() {

    clear_bullet_area();
    if (game_state.bullet_active) {
        draw_rect(game_state.bullet_x, game_state.bullet_y, 
                  game_state.bullet_x + BOX_SIZE, game_state.bullet_y + BOX_SIZE, 
                  BLUE, 1);
    }
    
    // Draw target (red box)
    draw_rect(game_state.target_x, game_state.target_y,
              game_state.target_x + BOX_SIZE, game_state.target_y + BOX_SIZE,
              RED, 1);
    

    if (game_state.score != game_state.last_score || game_state.bullet_speed != game_state.last_bullet_speed) {
        clear_score_area();
        char info_str[64];
        sprintf(info_str, "Score: %d | Speed: %d", game_state.score, game_state.bullet_speed);
        draw_string(UI_X, UI_Y, info_str, WHITE, 2);
        game_state.last_score = game_state.score;
        game_state.last_bullet_speed = game_state.bullet_speed;
    }
}

static int check_collision() {
    // Only check collision if bullet is active
    if (!game_state.bullet_active) return 0;
    
    // Check if bullet box overlaps with target box
    if (game_state.bullet_x < game_state.target_x + BOX_SIZE &&
        game_state.bullet_x + BOX_SIZE > game_state.target_x &&
        game_state.bullet_y < game_state.target_y + BOX_SIZE &&
        game_state.bullet_y + BOX_SIZE > game_state.target_y) {
        return 1;
    }
    return 0;
}

static void update_game() {
    // Only move bullet if it's active
    if (game_state.bullet_active) {
        // Save current position as previous
        game_state.prev_bullet_x = game_state.bullet_x;
        game_state.prev_bullet_y = game_state.bullet_y;
        // Move bullet right
        game_state.bullet_x += game_state.bullet_speed;
        
        // Update collision timer
        game_state.collision_timer++;
        
        // Only check collision when timer is up
        if (game_state.collision_timer >= game_state.collision_delay) {
            game_state.collision_timer = 0;  // Reset timer
            
            // Check if bullet hit target
            if (check_collision()) {
                game_state.score++;
                game_state.bullet_active = 0;  // Deactivate bullet
            }
        }
        
        // Check if bullet went off screen
        if (game_state.bullet_x > PHYSICAL_WIDTH) {
            game_state.bullet_active = 0;  // Deactivate bullet
        }
    }
}

void bullet_set_position(int x, int y) {
    game_state.bullet_x = x;
    game_state.bullet_y = y;
    game_state.prev_bullet_x = x;
    game_state.prev_bullet_y = y;
    game_state.bullet_active = 1;
}

void bullet_game() {
    // Initialize frame buffer
    framebf_init();
    
    // Initialize game state
    init_game();
    
    // Display initial instructions
    clear_screen();
    draw_string(PHYSICAL_WIDTH/2 - 200, PHYSICAL_HEIGHT/2 - 50, "Controls:", WHITE, 2);
    draw_string(PHYSICAL_WIDTH/2 - 200, PHYSICAL_HEIGHT/2, "SPACE: Shoot", WHITE, 2);
    draw_string(PHYSICAL_WIDTH/2 - 200, PHYSICAL_HEIGHT/2 + 50, "+/-: Change Speed", WHITE, 2);
    draw_string(PHYSICAL_WIDTH/2 - 200, PHYSICAL_HEIGHT/2 + 100, "Q: Quit", WHITE, 2);
    
    // Wait a moment to show instructions
    for (volatile int i = 0; i < 2000000; i++);
    
    while (!game_state.game_over) {
        // Clear screen
        clear_screen();
        
        // Check for input
        if (uart_isReadByteReady()) {
            char c = getUart();
            switch(c) {
                case ' ':
                    // Only shoot if no bullet is active
                    if (!game_state.bullet_active) {
                        bullet_set_position(50, PHYSICAL_HEIGHT / 2);
                    }
                    break;
                case '+':
                    if (game_state.bullet_speed < 20) {  // Max speed limit
                        game_state.bullet_speed++;
                    }
                    break;
                case '-':
                    if (game_state.bullet_speed > 1) {   // Min speed limit
                        game_state.bullet_speed--;
                    }
                    break;
                case 'q':
                    game_state.game_over = 1;
                    break;
            }
        }
        
        // Update game state
        update_game();
        
        // Draw game elements
        draw_game();
        
        // Small delay to control game speed
        for (volatile int i = 0; i < 1000000; i++);
    }
    
    // Game over screen
    clear_screen();
    draw_string(PHYSICAL_WIDTH/2 - 100, PHYSICAL_HEIGHT/2, "Game Over!", RED, 3);
    char final_score[64];
    sprintf(final_score, "Final Score: %d | Final Speed: %d", game_state.score, game_state.bullet_speed);
    draw_string(PHYSICAL_WIDTH/2 - 150, PHYSICAL_HEIGHT/2 + 50, final_score, WHITE, 2);
}
