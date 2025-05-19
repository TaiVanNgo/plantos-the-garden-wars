#include "../../include/plants.h"
#include "../assets/backgrounds/garden.h"

/**
 * Draw a plant at the specified grid position
 * 
 * @param plant_type The type of plant to draw (use PLANT_TYPE constants)
 * @param col Column in the grid (0-based)
 * @param row Row in the grid (0-based, corresponds to lanes)
 */
void draw_plant(int plant_type, int col, int row) {
    // Check if the column and row are within valid bounds
    if (col < 0 || col >= PLANT_GRID_COLS || row < 0 || row >= PLANT_GRID_ROWS) {
        return; // Invalid position, do nothing
    }
    
    // Calculate pixel coordinates from grid position, centered in cell
    int horizontal_offset = (PLANT_COL_WIDTH - PLANT_WIDTH) / 2;
    int vertical_offset = (PLANT_ROW_HEIGHT - PLANT_HEIGHT) / 2;
    int x = PLANT_GRID_LEFT_MARGIN + (col * PLANT_COL_WIDTH) + horizontal_offset;
    int y = PLANT_GRID_TOP_MARGIN + (row * PLANT_ROW_HEIGHT) + vertical_offset;
    
    const unsigned int* plant_sprite;
    
    switch(plant_type) {
        case PLANT_TYPE_PEASHOOTER:
            plant_sprite = peashooter;
            break;
        case PLANT_TYPE_SUNFLOWER:
            plant_sprite = sunflower;
            break;
        case PLANT_TYPE_SUNFLOWER_UNHAPPY:
            plant_sprite = sunflower_unhappy;
            break;
        case PLANT_TYPE_FROZEN_PEASHOOTER:
            plant_sprite = frozen_peashooter;
            break;
        case PLANT_TYPE_CHILLIES:
            plant_sprite = chillies;
            break;
        case PLANT_TYPE_CHILLIES_UNHAPPY:
            plant_sprite = chillies_unhappy;
            break;
        case PLANT_TYPE_WALLNUT:
            plant_sprite = wallnut;
            break;
        case PLANT_TYPE_WALLNUT_UNHAPPY:
            plant_sprite = wallnut_unhappy;
            break;
        default:
            // Handle invalid plant type
            return;
    }
    
    draw_image(plant_sprite, x, y, PLANT_WIDTH, PLANT_HEIGHT, 0);
}

/**
 * Draw the plant grid with gridlines and margins
 */
void draw_plant_grid(void) {
    unsigned int grid_color = 0x002299FF;    
    unsigned int margin_color = 0x00FF0000;  
    
    // Draw horizontal grid lines
    for (int i = 0; i <= PLANT_GRID_ROWS; i++) {
        draw_line(
            PLANT_GRID_LEFT_MARGIN, 
            PLANT_GRID_TOP_MARGIN + (i * PLANT_ROW_HEIGHT), 
            GARDEN_WIDTH, 
            PLANT_GRID_TOP_MARGIN + (i * PLANT_ROW_HEIGHT), 
            grid_color
        );
    }
    
    // Top margin (horizontal line)
    for (int i = 0; i < 3; i++) {
        draw_line(
            0, 
            PLANT_GRID_TOP_MARGIN + i, 
            GARDEN_WIDTH, 
            PLANT_GRID_TOP_MARGIN + i, 
            margin_color
        );
    }
    
}