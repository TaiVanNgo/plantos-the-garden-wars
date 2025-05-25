#include "../../include/buttons.h"

void button_init(Button *button, int x, int y, int width, int height, const unsigned int pixel_data[])
{
  button->x = x;
  button->y = y;
  button->width = width;
  button->height = height;
  button->state = BUTTON_NORMAL;
  button->pixel_data = pixel_data;
  draw_image(button->pixel_data, button->x, button->y, button->width, button->height, 0);
}

void button_handle_keypress(Button *buttons[], int num_buttons, int current_button, char key)
{
}

void button_set_state(Button *button, int state)
{
  button->state = state;
}

/**
 * Draws a selection cursor next to a button and restore background
 *
 * @param bg_type Background type to restore:
 *        0 = simulated_background
 *        1 = GARDEN
 *        2 = MAIN_SCREEN
 *        3 = tmp
 *        4 = VICTORY_SCREEN
 */
void button_draw_selection(Button **button, int current_selection, int prev_selection,
                           int bg_type, int x_offset, int y_offset)
{
  restore_background_area(
      button[prev_selection]->x - x_offset,
      button[prev_selection]->y + y_offset,
      ARROW_WIDTH, ARROW_HEIGHT,
      bg_type);
  draw_image(ARROW, button[current_selection]->x - x_offset, button[current_selection]->y + y_offset, ARROW_WIDTH, ARROW_HEIGHT, 0);
}

int button_is_selected(Button *button)
{
  return (button->state == BUTTON_SELECTED);
}