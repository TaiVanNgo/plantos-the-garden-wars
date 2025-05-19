#include "../include/uart0.h"
#include "../include/uart1.h"
// #include "../include/mbox.h"
#include "../include/framebf.h"
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
    int previous_selection= current_selection;
    // Initially set the first button selected
    button_set_state(buttons[current_selection], BUTTON_SELECTED);
    button_draw_selection(buttons, current_selection,previous_selection);

    while (1) {
        char key = getUart();
    
        if (key == 'W' || key == 'w') {
            int previous_selection = current_selection;
    
            // Clear previous button selection visually
            button_set_state(buttons[current_selection], BUTTON_NORMAL);
            // restore_background_area(buttons[previous_selection]->x, buttons[previous_selection]->y,
            //                        buttons[previous_selection]->width, buttons[previous_selection]->height);
    
            current_selection--;
            if (current_selection < 0) {
                current_selection = 1;
            }
    
            button_set_state(buttons[current_selection], BUTTON_SELECTED);
            button_draw_selection(buttons, current_selection, previous_selection);
        }
        else if (key == 'S' || key == 's') {
            int previous_selection = current_selection;
    
            button_set_state(buttons[current_selection], BUTTON_NORMAL);
            // restore_background_area(buttons[previous_selection]->x, buttons[previous_selection]->y,
            //                        buttons[previous_selection]->width, buttons[previous_selection]->height);
    
            current_selection++;
            if (current_selection > 1) {
                current_selection = 0;
            }
    
            button_set_state(buttons[current_selection], BUTTON_SELECTED);
            button_draw_selection(buttons, current_selection, previous_selection);
        }
        else if (key == '\n') {
            if (current_selection == 0) {
                uart_puts("start game ");
            }
            else if (current_selection == 1) {
                uart_puts("end game ");
                // break;  
            }
        }
    

    }
}
