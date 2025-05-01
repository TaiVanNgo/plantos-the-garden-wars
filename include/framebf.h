// ----------------------------------- framebf.h -------------------------------------
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
#define THISTLE 0xD8BFD8	
#define PULSE_CIRCLE_COLOR 0x000080

#define PHYSICAL_WIDTH 800
#define PHYSICAL_HEIGHT 600
#define VIRTUAL_WIDTH 800
#define VIRTUAL_HEIGHT 600

void framebf_init();
void draw_pixel(int x, int y, unsigned int attr);
void draw_rect(int x1, int y1, int x2, int y2, unsigned int attr, int fill);
void draw_line(int x1, int y1, int x2, int y2, unsigned int attr);
void draw_circle(int center_x, int center_y, int radius, unsigned int attr, int fill);
void draw_image(const unsigned int pixel_data[], int pos_x, int pos_y, int width, int height);
void clear_screen();
void draw_char(unsigned char ch, int x, int y, unsigned int attr);
void draw_string(int x, int y, char *s, unsigned int attr);