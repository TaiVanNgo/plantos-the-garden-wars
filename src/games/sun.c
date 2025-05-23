#include "../../include/sun.h"

extern const unsigned int sun[];

// Array to hold all active suns
static Sun suns[MAX_SUNS];

// Array to track sunflower positions
typedef struct {
    int col;
    int row;
    int active;
    int last_generation_frame;
} SunflowerTracker;

static SunflowerTracker sunflower_trackers[GRID_ROWS * GRID_COLS];
static int num_sunflowers = 0;
static unsigned long start_time;

// Initialize the sun system
void sun_system_init(unsigned long start_time_ms) {
    start_time = start_time_ms;
    
    // Initialize all suns as inactive
    for (int i = 0; i < MAX_SUNS; i++) {
        suns[i].active = 0;
    }
    
    // Initialize sunflower trackers
    for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {
        sunflower_trackers[i].active = 0;
    }
    
    uart_puts("[Sun] System initialized\n");
}

// Register a sunflower at the given position
void register_sunflower(int col, int row, int current_frame) {
    for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {
        if (!sunflower_trackers[i].active) {
            sunflower_trackers[i].col = col;
            sunflower_trackers[i].row = row;
            sunflower_trackers[i].active = 1;
            sunflower_trackers[i].last_generation_frame = current_frame;
            num_sunflowers++;
            uart_puts("[Sun] Registered sunflower at col=");
            uart_dec(col);
            uart_puts(", row=");
            uart_dec(row);
            uart_puts("\n");
            return;
        }
    }
}

// Create a new sun from a sunflower
void create_sun_from_sunflower(int col, int row, int current_frame) {
    // Find an inactive sun slot
    for (int i = 0; i < MAX_SUNS; i++) {
        if (!suns[i].active) {
            int x, y;
            grid_to_pixel(col, row, &x, &y);
            
            // Position sun at the plant's position
            int offset_x, offset_y;
            calculate_grid_center_offset(PLANT_WIDTH, PLANT_HEIGHT, &offset_x, &offset_y);
            
            x += offset_x + 15; // Offset slightly from plant center
            y += offset_y - 10; // Position above the plant
            
            suns[i].x = x;
            suns[i].y = y;
            suns[i].active = 1;
            suns[i].frame_created = current_frame;
            suns[i].plant_col = col;
            suns[i].plant_row = row;
            
            uart_puts("[Sun] Created sun from sunflower at col=");
            uart_dec(col);
            uart_puts(", row=");
            uart_dec(row);
            uart_puts("\n");
            
            return;
        }
    }
}

// Update all active suns
void update_suns(int current_frame) {
    // Check if any sunflowers need to generate suns
    for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {
        if (sunflower_trackers[i].active) {
            // If it's time to generate a sun (every SUN_GENERATION_TIME frames)
            if (current_frame - sunflower_trackers[i].last_generation_frame >= SUN_GENERATION_TIME) {
                create_sun_from_sunflower(sunflower_trackers[i].col, sunflower_trackers[i].row, current_frame);
                sunflower_trackers[i].last_generation_frame = current_frame;
            }
        }
    }
    
    // Update existing suns
    for (int i = 0; i < MAX_SUNS; i++) {
        if (suns[i].active) {
            // Check if the sun has expired
            if (current_frame - suns[i].frame_created >= SUN_LIFETIME) {
                // Remove the sun from the display
                restore_background_area(suns[i].x, suns[i].y, SUN_WIDTH, SUN_HEIGHT, 0, 0);
                suns[i].active = 0;
                uart_puts("[Sun] Sun disappeared\n");
            }
        }
    }
}

// Draw all active suns
void draw_suns() {
    for (int i = 0; i < MAX_SUNS; i++) {
        if (suns[i].active) {
            draw_image(sun, suns[i].x, suns[i].y, SUN_WIDTH, SUN_HEIGHT, 0);
        }
    }
}