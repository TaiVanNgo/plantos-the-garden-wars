#ifndef BULLET_H
#define BULLET_H

#include "../include/framebf.h"
#include "../include/framebf.h"
#include "../include/uart0.h"
#include "../include/plants.h"
#include "../assets/backgrounds/garden.h"
#include "../include/zombies.h"
#include "../include/plants.h"
#define BOX_SIZE 20
#define DEFAULT_BULLET_SPEED 5
#define DEFAULT_COLLISION_DELAY 10
#define UI_X 10
#define UI_Y 10
#define UI_W 350
#define UI_H 40
#define MAX_BULLETS 5
// #define MAX_BULLETS 6 // Uncomment for 6 bullets

// Remove old struct bullets and extern declaration
// struct bullets {
//     int bullet_x[MAX_BULLETS];
//     int bullet_y[MAX_BULLETS];
//     int prev_bullet_x[MAX_BULLETS];
//     int prev_bullet_y[MAX_BULLETS];
//     int target_x;
//     int target_y;
//     int score;
//     int last_score;
//     int game_over;
//     int bullet_speed;
//     int last_bullet_speed;
//     int collision_timer;
//     int collision_delay;
//     int bullet_active[MAX_BULLETS];
// };
// extern struct bullets bullets;

#define MAX_BULLETS 25 // 5 rows * 5 bullets per row
#define MAX_PLANTS 10

// --- Data Structures ---
typedef struct {
    int x, y;
    int prev_x, prev_y;
    int row;
    int active;
    int plant_type;
} Bullet;

typedef struct {
    int col, row;
    unsigned long last_fire_time;
} PlantInstance;


void check_bullet_zombie_collisions(Zombie *zombie);
void apply_bullet_damage(Bullet *bullet, Zombie *zombie);

#endif // BULLET_H 