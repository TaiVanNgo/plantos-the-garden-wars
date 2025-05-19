#include "../include/uart0.h"
#include "../include/uart1.h"
// #include "../include/mbox.h"
// #include "../include/framebf.h"
// #include "../include/cmd.h"
// #include "../include/video.h"
// #include "../include/utils.h"
#include "../assets/backgrounds/background.h"
#include "../assets/button/button.h"

void game_init() {
    draw_image(BACKGROUND, 0, 0, 800, 600, 0);

    Button startButton;
    Button endButton;
    button_init(&startButton, 240, 300, 300, 85, START);
    button_init(&endButton, 240, 400, 300, 85, QUIT);

    Button* buttons[2] = { &startButton, &endButton };
    int current_selection = 0;

    // Initially set the first button selected
    button_set_state(buttons[current_selection], BUTTON_SELECTED);
    button_draw_selection(buttons[current_selection]);

    while (1) {
        char key = getUart();  

        if (key == 'W' || key == 'w') {  
            button_set_state(buttons[current_selection], BUTTON_NORMAL);
            current_selection--;
            if (current_selection < 0) {
                current_selection = 1;
            }
            button_set_state(buttons[current_selection], BUTTON_SELECTED);
        }
        else if (key == 's' || key == 'S') { 
            button_set_state(buttons[current_selection], BUTTON_NORMAL);
            current_selection++;
            if (current_selection > 1) {
                current_selection = 0; 
            }
            button_set_state(buttons[current_selection], BUTTON_SELECTED);
        }
        else if (key == '\n') {  
            if (current_selection == 0) {
                // Start game
            }
            else if (current_selection == 1) {
                // Quit game
                break;
            }
        }

        // Redraw buttons (you may want to clear screen or redraw background here)
        // draw_image(BACKGROUND, 0, 0, 800, 600, 0);

        for (int i = 0; i < 2; i++) {
            button_init(buttons[i], buttons[i]->x, buttons[i]->y, buttons[i]->width, buttons[i]->height, buttons[i]->pixel_data);
        }

        // Draw arrow next to selected button
        button_draw_selection(buttons[current_selection]);
    }
}
