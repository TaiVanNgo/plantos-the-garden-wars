#include "../include/uart0.h"
#include "../include/uart1.h"
// #include "../include/mbox.h"
#include "../include/framebf.h"
// #include "../include/cmd.h"
// #include "../include/video.h"
// #include "../include/utils.h"
#include "../assets/backgrounds/background.h"
#include "../assets/button/button.h"
#include "../../include/game_init.h"
void game_start() {
    draw_image(MAIN_SCREEN, 0, 0,  BACKGROUND_WIDTH,BACKGROUND_HEIGHT, 0);

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
        if(key == '['){
            char key2= getUart();
            if ((key2 == 'A' ) && GAME_START== 0 ) {
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
            else if ((key2 == 'B' ) && GAME_START== 0 ) {
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
        }
       

        if (key == '\n' && GAME_START== 0 ) {
            if (current_selection == 0) {
                GAME_START = 1; 
                game_init();
            }
            else if (current_selection == 1) {
                uart_puts("end game ");
                // break;  
            }
        }
    

    }
}


void game_init()
{
   
  draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
//   draw_image()
//   draw_rect(0, 70, 800, 70, RED, 0);
//   draw_rect(0, 184, 800, 272, BLUE, 0);

//   Zombie zombie1 = spawn_zombie(1, 1);
//   Zombie zombie2, zombie3, zombie4, zombie5;
//   int zombie2_spawned = 0;
//   int zombie3_spawned = 0;
//   int zombie4_spawned = 0;
//   int zombie5_spawned = 0;

//   int cnt = 0;

//   // Game loop running at 10FPS
//   while (1)
//   {
//     set_wait_timer(1, 17); // Round up to 17ms for simplicity

//     update_zombie_position(&zombie1);

//     if (cnt >= 50 && !zombie2_spawned)
//     {
//       zombie2 = spawn_zombie(2, 2);
//       zombie2_spawned = 1;
//     }

//     if (cnt >= 100 && !zombie3_spawned)
//     {
//       zombie3 = spawn_zombie(3, 3);
//       zombie3_spawned = 1;
//     }

//     if (cnt >= 150 && !zombie4_spawned)
//     {
//       zombie4 = spawn_zombie(1, 4);
//       zombie4_spawned = 1;
//     }

//     if (cnt >= 200 && !zombie5_spawned)
//     {
//       zombie5 = spawn_zombie(2, 1);
//       zombie5_spawned = 1;
//     }

//     if (zombie2_spawned)
//     {
//       update_zombie_position(&zombie2);
//     }

//     if (zombie3_spawned)
//     {
//       update_zombie_position(&zombie3);
//     }

//     if (zombie4_spawned)
//     {
//       update_zombie_position(&zombie4);
//     }

//     if (zombie5_spawned)
//     {
//       update_zombie_position(&zombie5);
//     }
//     cnt++;

//     // Wait until the 100ms timer expires
//     set_wait_timer(0, 0); // Second parameter is ignored in wait mode
//   }
}
