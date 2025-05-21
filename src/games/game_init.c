#include "../include/uart0.h"
#include "../include/uart1.h"
// #include "../include/mbox.h"
#include "../include/framebf.h"
// #include "../include/cmd.h"
// #include "../include/video.h"
// #include "../include/utils.h"
#include "../assets/backgrounds/background.h"
#include "../assets/selection/selection.h"
#include "../assets/button/button.h"
#include "../../include/game_init.h"
#include "../include/plants.h"
#define GRID_START_X 90     
#define GRID_START_Y 178   
#define GRID_CELL_WIDTH 80  
#define GRID_CELL_HEIGHT 85 
#define GRID_ROWS 4       
#define GRID_COLS 9        

// Card positions for plant selection
int CARD_START_X =50;    // Left edge of first card
int CARD_START_Y =178;    // Top edge of cards
#define CARD_WIDTH 50       // Width of each card
#define CARD_HEIGHT 70      // Height of each card
#define CARD_COUNT 8        // Number of plant cards

// Tracks whether we're in card selection mode or grid placement mode
int selection_mode = 0;     // 0 = card selection, 1 = grid placement
int selected_card = -1; 
int selected_row = 1;
int selected_col = 1;


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
                clear_screen();
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


// void game_init()
// {
   
//   draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
// //   draw_image(selection,100,100,577,433,0);
//     // draw_image(selection, 100,100, 577,433,0);
//     draw_rect(50,178,115,260, 0xff0FFF, 0);
//     char c= getUart();
//     while(1){
//         if(c == '['){
//             if(c == 'A'){
//                 // int previous_selection = current_selection;
//             }
//         }
//     }
//   577,433
//   draw_rect(0, 70, 800, 70, R433ED, 0);
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
// }

// void game_init() {

//     draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);

//     int selected_row = 0;
//     int selected_col = 0;

//     // Draw initial selection
//     draw_selection(selected_row, selected_col);
//     uart_puts("em ga oi");
//     while (1) {
//         char key = getUart();
//         if(key == '['){
//             char key2= getUart();
//             if ((key2 == 'A' )  ) {
//                 uart_puts("em ga oi");
//                 if (selected_row > 0){
//                     selected_row++;
//                 } 
//             }
//             else if ((key2 == 'B' )  ) {
//                 uart_puts("em ga oi");
//                 if (selected_row > 0){
//                     selected_row--;
//                 } 
//             }else if((key2 == 'C')){
//                 uart_puts("em ga oi");
//                 if (selected_col > 0){
//                     selected_col++;
//                 } 
//             }else if((key2 == 'D')){
//                 uart_puts("em ga oi");
//                 if (selected_col > 0){
//                     selected_col--;
//                 } 
//             }
//         }
       

//         if (key == '\n' && GAME_START== 0 ) {
           
//         }
    

//     }
// }

// void draw_selection(int row, int col) {
//     // Clear previous selection (optional)
//     // Redraw background grid cell under old selection if needed

//     int cell_width = 50;
//     int cell_height = 85;
//     int grid_start_x = 50;
//     int grid_start_y = 178;

//     int x = grid_start_x + col * cell_width;
//     int y = grid_start_y + row * cell_height;

//     // Draw the selection rectangle around the cell
//     draw_rect(x, y, cell_width, cell_height, 0xff0000, 1);  // For example, red border
// }


void draw_selection(int row, int col) {
    static int prev_row = -1;
    static int prev_col = -1;
    static int prev_mode = -1;
   
    // If we have a previous selection, restore that area
    if (prev_row != -1 && prev_col != -1) {
        if (prev_mode == 0) {
            // Restore card selection area
            int x = CARD_START_X + prev_col * CARD_WIDTH;
            int y = CARD_START_Y;
            restore_background_area(x - 4, y - 4, CARD_WIDTH + 8, CARD_HEIGHT + 8, 0);
        } else {
            // Restore grid cell area
            int x = GRID_START_X + prev_col * GRID_CELL_WIDTH;
            int y = GRID_START_Y + prev_row * GRID_CELL_HEIGHT;
            restore_background_area(x - 4, y - 4, GRID_CELL_WIDTH + 8, GRID_CELL_HEIGHT + 8, 1);
        }
    }
    
    // Draw new selection
    if (selection_mode == 0) {
        // Draw selection around a plant card
        int x = CARD_START_X + col * CARD_WIDTH;
        int y = CARD_START_Y;
        
        // Draw a yellow highlight around the card
        draw_rect(x - 3, y - 3, CARD_WIDTH + 6, CARD_HEIGHT + 6, 0xFFFF00, 0);
    } else {
        // Draw selection on grid for placement
        int x = GRID_START_X + col * GRID_CELL_WIDTH;
        int y = GRID_START_Y + row * GRID_CELL_HEIGHT;
        
        // Draw a white semi-transparent rectangle to show valid placement
        draw_rect(x, y, GRID_CELL_WIDTH, GRID_CELL_HEIGHT, 0x80FFFFFF, 0);
        
        // If we have a selected card, show the plant preview
        if (selected_card >= 0) {
            // This would show a preview of the plant
            // You'll need plant sprites for this
            // For now, just draw a colored rectangle
            draw_rect(x + 15, y + 15, 50, 50, 0x8000FF00, 1); 
        }
    }
    
    // Remember current selection for next time
    prev_row = row;
    prev_col = col;
    prev_mode = selection_mode;
}


void game_init() {
    Zombie zombie= spawn_zombie(1,1);

    draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    
    // Plant selection variables
    int selected_row = 0;
    int selected_col = 0;
    selection_mode = 0;     // Start in card selection mode
    selected_card = -1;     // No card selected initially
    static int offset= 20;
    // Draw initial selection (on first card)
    int current_selection = -1;
    // draw_selection(selected_row, selected_col);
    // draw_rect(CARD_START_X  ,CARD_START_Y , CARD_START_X +60, CARD_START_Y+75, 0xfF00ff, 0);
    // draw_plant(,selected_col, selected_row);
    
    while (1) {
        update_zombie_position(&zombie);
        int x=0;
    int y=0;
        char key = getUart();
        switch (key) {
            case '0': current_selection = 0; break;
            case '1': current_selection = 1; break;
            case '2': current_selection = 2; break;
            case '3': current_selection = 3; break;
            case '4': current_selection = 4; break;
            case '5': current_selection = 5; break;
            case '6': current_selection = 6; break;
            default:
                break;
        }
        if (current_selection != -1) {
            grid_to_pixel(selected_col,selected_row, &x,&y );

        }
        

        if (key == '[') {
            char key2 = getUart();
            if (key2 == 'A') { // Up arrow
                // CARD_START_Y+= 90;
                if(selected_row <= 0){
                    selected_row=0;
                }else{
                    selected_row--;
                }
                
                if(current_selection!= -1){
                    restore_background_area(x, y, GRID_CELL_WIDTH,GRID_CELL_HEIGHT,0);
                    
                    draw_plant(current_selection,selected_col, selected_row);
                }
              
               

            }
            else if (key2 == 'B') { 
                if(selected_row >= 4){
                    selected_row=4;
                }else{
                    selected_row++;
                }
                if(current_selection!= -1){
                    restore_background_area(x, y, GRID_CELL_WIDTH,GRID_CELL_HEIGHT,0);
                    // grid_to_pixel(selected_col,selected_row, &x, &y );
                    draw_plant(current_selection,selected_col, selected_row);
                }
               
            }
            else if (key2 == 'C') { 
                if(selected_col >= 9){
                    selected_col=9;
                }else{
                    selected_col++;
                }
                if(current_selection!= -1){
                    restore_background_area(x, y, GRID_CELL_WIDTH,GRID_CELL_HEIGHT,0);
                    // grid_to_pixel(selected_col,selected_row, x, y );
                    draw_plant(current_selection,selected_col, selected_row);
                }

            }
            else if (key2 == 'D') { // Left arrow
                if(selected_col <= 0){
                    selected_col=1;
                }else{
                    selected_col--;
                }
                if(current_selection!= -1){
                    restore_background_area(x, y, GRID_CELL_WIDTH,GRID_CELL_HEIGHT,0);
                    // grid_to_pixel(selected_col,selected_row, x, y );
                    draw_plant(current_selection,selected_col, selected_row);
                }

            }
            uart_puts("\n X:");
            uart_dec(x);
            uart_puts("\n Y :");
            uart_dec(y);
            uart_puts("\n");
        }
        
        if (key == '\n') {
            if (selection_mode == 0) {
                // Select this plant card
                selected_card = selected_col;
                
                // Switch to grid placement mode
                selection_mode = 1;
                selected_row = 0;
                selected_col = 0;
                draw_selection(selected_row, selected_col);
                
                // uart_puts("Selected plant: ");
                // uart_putc('0' + selected_card);
                // uart_puts("\n");
            } else {
                // Place the plant at the selected grid position
                // place_plant(selected_card, selected_row, selected_col);
                
                // Return to card selection mode
                selection_mode = 0;
                selected_card = -1;
                selected_col = 0;
                draw_selection(selected_row, selected_col);
                
                // uart_puts("Placed plant at row ");
                // uart_putc('0' + selected_row);
                // uart_puts(", col ");
                // uart_putc('0' + selected_col);
                // uart_puts("\n");
            }
        }
        
        if (key == '0') { // Deselect (cancel)
            // Return to card selection mode
            selection_mode = 0;
            selected_card = -1;
            selected_col = 0;
            draw_selection(selected_row, selected_col);
        }
        delay_ms(100);
    }
}


