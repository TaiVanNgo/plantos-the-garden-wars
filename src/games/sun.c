#include "sun.h"
#include "game_init.h"

extern const unsigned int sun[];
static Sun suns[MAX_SUNS];
static SunflowerTracker sunflower_trackers[GRID_ROWS * GRID_COLS];
static int num_sunflowers = 0;
static unsigned long start_time;
extern GameState game;

static int warning_active = 0;       // Flag to track if warning is active
static int warning_start_frame = 0;  // Frame when warning started
static int WARNING_DURATION = 50;    // Duration in frames (about 1 second)

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

// Unregister a sunflower at the given position
void unregister_sunflower(int col, int row) {
    for (int i = 0; i < GRID_ROWS * GRID_COLS; i++) {
        if (sunflower_trackers[i].active &&
            sunflower_trackers[i].col == col &&
            sunflower_trackers[i].row == row) {
            sunflower_trackers[i].active = 0;
            num_sunflowers--;

            uart_puts("[Sun] Unregistered sunflower at col=");
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

            // Position sun at the bottom left corner of the cell
            x += 10;  // Small margin from left edge

            // Position at the bottom with a small margin
            y += GRID_ROW_HEIGHT - SUN_HEIGHT - 10;

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
            if (current_frame - sunflower_trackers[i].last_generation_frame >= SUN_GENERATION_TIME) {
                create_sun_from_sunflower(sunflower_trackers[i].col, sunflower_trackers[i].row, current_frame);
                sunflower_trackers[i].last_generation_frame = current_frame;
            }
        }
    }

    // Check if warning needs to be cleared
    if (warning_active && (current_frame - warning_start_frame >= WARNING_DURATION)) {
        warning_active = 0;
        draw_sun_count_enhanced(game.sun_count, BROWN, 1, 1);
        uart_puts("[Sun] Warning cleared\n");
    }

    // Update existing suns
    for (int i = 0; i < MAX_SUNS; i++) {
        if (suns[i].active) {
            // Check if the sun has expired
            if (current_frame - suns[i].frame_created >= SUN_LIFETIME) {
                // Remove the sun from the display
                restore_background_area(suns[i].x, suns[i].y, SUN_WIDTH, SUN_HEIGHT, 0);
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

int collect_sun_at_position(int col, int row) {
    int cell_x, cell_y;
    grid_to_pixel(col, row, &cell_x, &cell_y);

    // Define the grid cell boundaries
    int cell_right = cell_x + GRID_COL_WIDTH;
    int cell_bottom = cell_y + GRID_ROW_HEIGHT;

    // Check all active suns to see if they're at this position
    for (int i = 0; i < MAX_SUNS; i++) {
        if (!suns[i].active)
            continue;

        // Define the sun boundaries
        int sun_right = suns[i].x + SUN_WIDTH;
        int sun_bottom = suns[i].y + SUN_HEIGHT;

        // Check if the sun overlaps with the grid cell
        if (!(cell_x > sun_right || cell_right < suns[i].x ||
              cell_y > sun_bottom || cell_bottom < suns[i].y)) {
            // Sun overlaps with the cell - collect it
            game.sun_count += SUN_VALUE;
            // Remove the sun from the display
            restore_background_area(suns[i].x, suns[i].y, SUN_WIDTH, SUN_HEIGHT, 0);

            // Mark the sun as inactive
            suns[i].active = 0;

            // Update the sun count display
            draw_sun_count(game.sun_count);

            uart_puts("[Sun] Collected sun! +50 sun resources\n");
            uart_puts("[Sun] New sun count: ");
            uart_dec(game.sun_count);
            uart_puts("\n");

            return 1;  // Sun collected successfully
        }
    }

    return 0;  // No sun collected
}

void draw_sun_count(int count) {
    draw_sun_count_enhanced(count, BROWN, 1, 0);
}

void draw_sun_count_enhanced(int count, int color, int size, int force_update) {
    static int last_count = -1;

    // Only redraw if count changed or forced update
    if (count != last_count || force_update) {
        // Clear a larger area for the text
        restore_background_area(SUN_COUNT_X - 30, SUN_COUNT_Y - 30, 250, 100, 0);

        char count_str[10];
        int_to_str(count, count_str);

        // Adjust Y position based on text size to center it vertically
        if (size > 1) {
            draw_string(SUN_COUNT_X, SUN_COUNT_Y - 5, count_str, color, size);
        } else {
            draw_string(SUN_COUNT_X + 10, SUN_COUNT_Y, count_str, color, size);
        }

        last_count = count;
    }
}

void trigger_insufficient_sun_warning(int current_frame) {
    // Set warning state
    warning_active = 1;
    warning_start_frame = current_frame;

    // Display sun count in red and larger
    draw_sun_count_enhanced(game.sun_count, RED, 2, 1);
}