#ifndef BUTTON_H
#define BUTTON_H

#define BUTTON_NORMAL 0
#define BUTTON_SELECTED 1
#define BUTTON_PRESSED 2

#define ARROW_WIDTH 58
#define ARROW_HEIGHT 72
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT 85

#include "../assets/button/button_sprite.h"
#include "framebf.h"

// Button structure
typedef struct
{
    int x;
    int y;
    int width;
    int height;
    int state;
    unsigned int *pixel_data;
    void (*callback)();
} Button;

void button_init(Button *button, int x, int y, int width, int height, const unsigned int pixel_data[]);
void button_set_state(Button *button, int state);
void button_draw_selection(Button **button, int current_selection, int prev_selection, int bg_type, int x_offset, int y_offset);
int button_is_selected(Button *button);

#endif
