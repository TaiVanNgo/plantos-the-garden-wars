// ----------------------------------- framebf.h -------------------------------------
#include "../assets/fonts/fonts.h"
#include "../assets/backgrounds/garden.h"
#include "../assets/backgrounds/background.h"
#include "../assets/selection/selection.h"
#include "mbox.h"
#include "uart0.h"
#include "uart1.h"
#include "video.h"
#include "cooldown.h"
#include "plants.h"

#define RED 0xFF0000
#define SALMON_RED 0xFF6666
#define GREEN 0x00FF00
#define GREEN_YELLOW 0xADFF2F
#define BLUE 0x0000FF
#define BRIGHT_BLUE 0x0096FF
#define PURPLE 0x800080
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define LAVENDER 0xE6E6FA
#define THISTLE 0xBB8FBB
#define PULSE_CIRCLE_COLOR 0x000080
#define BROWN 0x890129
// Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32
// Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0

#define PHYSICAL_WIDTH 800
#define PHYSICAL_HEIGHT 600
#define VIRTUAL_WIDTH 800
#define VIRTUAL_HEIGHT 600

extern unsigned int simulated_background[GARDEN_WIDTH * GARDEN_HEIGHT];
extern unsigned int tmp[GARDEN_WIDTH * GARDEN_HEIGHT];
void framebf_init();
void draw_pixel(int x, int y, unsigned int attr);
void draw_rect(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void draw_line(int x1, int y1, int x2, int y2, unsigned int attr);
void draw_vline(int x, int y1, int y2, unsigned int attr);
double sqrt(double number);
void draw_circle(int center_x, int center_y, int radius, unsigned int attr, int fill);
void draw_image(const unsigned int pixel_data[], int pos_x, int pos_y, int width, int height, int draw_transparent);
void clear_screen();
void draw_char(unsigned char ch, int x, int y, unsigned int attr, int scale);
void draw_string(int x, int y, const char *s, unsigned int attr, int scale);
void restore_background_area(int x, int y, int width, int height, int bg_type);

void create_simulated_background(unsigned int *sim_bg, const unsigned int garden[], int garden_width, int garden_height);
void draw_on_simulated_background(unsigned int *sim_bg, const unsigned int plant[], int plant_x, int plant_y, int plant_width, int plant_height, int garden_width);
unsigned int get_simulated_pixel(const unsigned int *sim_bg, int x, int y, int garden_width);
void draw_image_scaled(const unsigned int *image_data, int x, int y, int src_width, int src_height, int dest_width, int dest_height, int transparent);
void clear_plant_from_background(int grid_col, int grid_row, int background, int taken);
void draw_selection_border(int selection);
void restore_grid_area_to_garden(int grid_col, int grid_row);
void draw_cooldown_on_cards(int plant_type);
void reset_tmp_region_from_garden(int grid_col, int grid_row);