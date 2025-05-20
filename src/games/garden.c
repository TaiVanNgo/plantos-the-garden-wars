#include "../include/garden.h"

// Convert grid position to pixel coordinates
void grid_to_pixel(uint8_t col, uint8_t row, int *x, int *y) {
    if (is_valid_grid_position(col, row)) {
        *x = GRID_LEFT_MARGIN + (col * GRID_COL_WIDTH);
        *y = GRID_TOP_MARGIN + (row * GRID_ROW_HEIGHT);
    } else {
        *x = 0;
        *y = 0;
    }
}

// Convert pixel coordinates to grid position
void pixel_to_grid(int x, int y, uint8_t *col, uint8_t *row) {
    if (is_in_grid(x, y)) {
        *col = (x - GRID_LEFT_MARGIN) / GRID_COL_WIDTH;
        *row = (y - GRID_TOP_MARGIN) / GRID_ROW_HEIGHT;
    } else {
        *col = 0xFF; // Invalid column
        *row = 0xFF; // Invalid row
    }
}

int is_valid_grid_position(uint8_t col, uint8_t row) {
    return (col < GRID_COLS && row < GRID_ROWS);
}

int is_in_grid(int x, int y) {
    return (x >= GRID_LEFT_MARGIN && 
            x < GRID_LEFT_MARGIN + (GRID_COLS * GRID_COL_WIDTH) &&
            y >= GRID_TOP_MARGIN && 
            y < GRID_TOP_MARGIN + (GRID_ROWS * GRID_ROW_HEIGHT));
}

/**
 * Calculate center position in a grid cell
 * 
 * @param col Column in the grid (0-based)
 * @param row Row in the grid (0-based)
 * @param center_x Pointer to store the center x coordinate
 * @param center_y Pointer to store the center y coordinate
 */
void get_grid_cell_center(uint8_t col, uint8_t row, int *center_x, int *center_y) {
    if (is_valid_grid_position(col, row)) {
        *center_x = GRID_LEFT_MARGIN + (col * GRID_COL_WIDTH) + (GRID_COL_WIDTH / 2);
        *center_y = GRID_TOP_MARGIN + (row * GRID_ROW_HEIGHT) + (GRID_ROW_HEIGHT / 2);
    } else {
        *center_x = 0;
        *center_y = 0;
    }
}

/**
 * Calculate the offset to center an object in a grid cell
 * 
 * @param object_width Width of the object to be centered
 * @param object_height Height of the object to be centered
 * @param offset_x Pointer to store the x offset
 * @param offset_y Pointer to store the y offset
 */
void calculate_grid_center_offset(int object_width, int object_height, int *offset_x, int *offset_y) {
    *offset_x = (GRID_COL_WIDTH - object_width) / 2;
    *offset_y = (GRID_ROW_HEIGHT - object_height) / 2;
}


void draw_grid(void) {
    unsigned int grid_color = 0x002299FF;    // Blue grid lines
    unsigned int margin_color = 0x00FF0000;  // Red margin indicator
    
    // Draw horizontal grid lines
    for (int i = 0; i <= GRID_ROWS; i++) {
        draw_line(
            GRID_LEFT_MARGIN, 
            GRID_TOP_MARGIN + (i * GRID_ROW_HEIGHT), 
            GARDEN_WIDTH, 
            GRID_TOP_MARGIN + (i * GRID_ROW_HEIGHT), 
            grid_color
        );
    }
    
    // Draw vertical grid lines
    for (int i = 0; i <= GRID_COLS; i++) {
        draw_line(
            GRID_LEFT_MARGIN + (i * GRID_COL_WIDTH),
            GRID_TOP_MARGIN,
            GRID_LEFT_MARGIN + (i * GRID_COL_WIDTH),
            GRID_TOP_MARGIN + (GRID_ROWS * GRID_ROW_HEIGHT),
            grid_color
        );
    }
    
    // Top margin indicator (horizontal line)
    for (int i = 0; i < 3; i++) {
        draw_line(
            0, 
            GRID_TOP_MARGIN + i, 
            GARDEN_WIDTH, 
            GRID_TOP_MARGIN + i, 
            margin_color
        );
    }
    
    // Left margin indicator (vertical line)
    for (int i = 0; i < 3; i++) {
        draw_line(
            GRID_LEFT_MARGIN + i,
            GRID_TOP_MARGIN,
            GRID_LEFT_MARGIN + i,
            GRID_TOP_MARGIN + (GRID_ROWS * GRID_ROW_HEIGHT),
            margin_color
        );
    }
}