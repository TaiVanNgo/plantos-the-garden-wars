#ifndef SUN_H
#define SUN_H

#include "grid.h"
#include "gpio.h"
#include "plants.h"


#define INITIAL_SUN_COUNT 400  
#define SUN_WIDTH 35
#define SUN_HEIGHT 35
#define MAX_SUNS 50
#define SUN_LIFETIME 300  // Sun exists for 300 frames (about 6 seconds at 50ms per frame)
#define SUN_GENERATION_TIME 250  // Generate sun every 250 frames (about 5 seconds at 50ms per frame)

typedef struct {
    int x;
    int y;
    int active;
    int frame_created;
    int plant_col;      // Associated plant column
    int plant_row;      // Associated plant row
} Sun;

// Initialize the sun system
void sun_system_init(unsigned long start_time_ms);

// Update all active suns
void update_suns(int current_frame);

// Draw all active suns
void draw_suns();

// Register a sunflower that will generate suns
void register_sunflower(int col, int row, int current_frame);
void unregister_sunflower(int col, int row);
int collect_sun_at_position(int col, int row);
void draw_sun_count(int count);
void draw_sun_count_enhanced(int count, int color, int size, int force_update);
void trigger_insufficient_sun_warning(int current_frame);

#endif // SUN_H