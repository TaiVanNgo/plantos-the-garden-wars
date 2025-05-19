#ifndef BULLET_H
#define BULLET_H

#include "../include/framebf.h"

#define BOX_SIZE 20
#define DEFAULT_BULLET_SPEED 5
#define DEFAULT_COLLISION_DELAY 10
#define UI_X 10
#define UI_Y 10
#define UI_W 350
#define UI_H 40
#define MAX_BULLETS 2
// #define MAX_BULLETS 6 // Uncomment for 6 bullets

struct bullets {
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
    int collision_timer;
    int collision_delay;
    int bullet_active;
};

extern struct bullets bullets;

void bullet_set_position(int x, int y);
void update_bullets(void);
void fire_bullet(void);
void update_all_bullets(void);
void draw_all_bullets(void);

#endif // BULLET_H 