#ifndef GARDEN_H
#define GARDEN_H

#define GRID_LEFT_MARGIN 45
#define GRID_TOP_MARGIN 180  // top margin
#define GRID_COL_WIDTH 78    // column width
#define GRID_ROW_HEIGHT 85
#define GRID_ROWS 4
#define GRID_COLS 9

#include "../assets/sprites/cursor.h"
#include "../assets/sprites/plants/plants_sprites.h"
#include "framebf.h"
#include "plants.h"

// Grid parameters
#define GARDEN_WIDTH 800
#define GARDEN_HEIGHT 600

#define CURSOR_WIDTH 17
#define CURSOR_HEIGHT 17
void grid_to_pixel(uint8_t col, uint8_t row, int *x, int *y);
void pixel_to_grid(int x, int y, uint8_t *col, uint8_t *row);
void draw_grid(void);
int is_valid_grid_position(uint8_t col, uint8_t row);
int is_in_grid(int x, int y);

// Calculate center position in a grid cell
void get_grid_cell_center(uint8_t col, uint8_t row, int *center_x, int *center_y);

// Calculate the offset to center an object in a grid cell
void calculate_grid_center_offset(int object_width, int object_height, int *offset_x, int *offset_y);

#endif