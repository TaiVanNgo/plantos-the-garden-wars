#ifndef BULLET_H
#define BULLET_H

#include "../include/framebf.h"
#include "../include/uart0.h"
#include "../include/plants.h"
#include "../assets/backgrounds/garden.h"
#include "../include/zombies.h"
#include "../include/utils.h"

// Constants
#define MAX_BULLETS 25 // 5 rows * 5 bullets per row
#define MAX_PLANTS 10
#define BULLET_WIDTH 20
#define BULLET_HEIGHT 20
#define BULLET_SPEED 3
#define BULLET_FIRE_INTERVAL 1000 // ms, default 1 second
#define BULLET_MOVE_INTERVAL 30   // ms

// Data Structures
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

// Function Declarations
void bullet_system_init(unsigned long start_ms, int fire_interval_ms);
void bullet_spawn_plant(int col, int row, unsigned long start_ms);
void bullet_remove_plant(int col, int row);
void bullet_update(unsigned long current_time_ms);
void bullet_draw(void);
void check_bullet_zombie_collisions(Zombie *zombie);
void apply_bullet_damage(Bullet *bullet, Zombie *zombie);
void spawn_peashooter(int col, int row, unsigned long current_time_ms);
void draw_plants_both(int plant_type, int col, int row);
void draw_image_both(const unsigned int pixel_data[], int pos_x, int pos_y, int width, int height, int show_transparent);
void bullet_game(void);
void register_zombie_on_row(int row, int active);
void reset_zombie_counts(void);

// External variables
extern const unsigned int bullet_green[];

#endif // BULLET_H 