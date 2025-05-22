#include "../../include/game_init.h"

// Card positions for plant selection
int CARD_START_X = 50;  // Left edge of first card
int CARD_START_Y = 178; // Top edge of cards

// Tracks whether we're in card selection mode or grid placement mode
int selection_mode = 0; // 0 = card selection, 1 = grid placement
int selected_card = -1;
int selected_row = 1;
int selected_col = 1;

GameState game = {.state = GAME_MENU, .score = 0, .level = LEVEL_EASY_ENUM};

void game_main()
{

    while (1)
    {
        switch (game.state)
        {
        case GAME_MENU:
            game.state = game_menu();
            break;
        case GAME_PLAYING:
            start_level();
            // start_level();
            break;
        case GAME_PAUSED:
            // Handle pause menu
            break;
        case GAME_OVER:
            // Show game over screen, handle restart/exit
            break;
        }
    }
}

GAME_STATE game_menu()
{
    draw_image(MAIN_SCREEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);

    Button startButton;
    Button endButton;
    button_init(&startButton, 240, 300, 300, 85, START);
    button_init(&endButton, 240, 400, 300, 85, QUIT);

    Button *buttons[2] = {&startButton, &endButton};
    int current_selection = 0;
    int previous_selection = current_selection;
    // Initially set the first button selected
    button_set_state(buttons[current_selection], BUTTON_SELECTED);
    button_draw_selection(buttons, current_selection, previous_selection);

    while (1)
    {
        char key = getUart();
        if (key == '[')
        {
            char key2 = getUart();
            if ((key2 == 'A'))
            {
                // 'up arrow' button
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);
                current_selection--;
                if (current_selection < 0)
                {
                    current_selection = 1;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection);
            }
            else if ((key2 == 'B'))
            {
                // 'down arrow' button
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);

                current_selection++;
                if (current_selection > 1)
                {
                    current_selection = 0;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection);
            }
        }

        if (key == '\n')
        {
            if (current_selection == 0)
            {
                clear_screen();
                return GAME_PLAYING;
            }
            else if (current_selection == 1)
            {
                uart_puts("Quit Game ");
                return GAME_QUIT;
            }
        }
    }
}

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

//         if (key == '\n'  ) {

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

void draw_selection(int row, int col)
{
    static int prev_row = -1;
    static int prev_col = -1;
    static int prev_mode = -1;

    // If we have a previous selection, restore that area
    if (prev_row != -1 && prev_col != -1)
    {
        if (prev_mode == 0)
        {
            // Restore card selection area
            int x = CARD_START_X + prev_col * CARD_WIDTH;
            int y = CARD_START_Y;
            restore_background_area(x - 4, y - 4, CARD_WIDTH + 8, CARD_HEIGHT + 8, 0);
        }
        else
        {
            // Restore grid cell area
            int x = GRID_LEFT_MARGIN + prev_col * GRID_COL_WIDTH;
            int y = GRID_TOP_MARGIN + prev_row * GRID_ROW_HEIGHT;
            restore_background_area(x - 4, y - 4, GRID_COL_WIDTH + 8, GRID_ROW_HEIGHT + 8, 0);
        }
    }

    // Draw new selection
    if (selection_mode == 0)
    {
        // Draw selection around a plant card
        int x = CARD_START_X + col * CARD_WIDTH;
        int y = CARD_START_Y;

        // Draw a yellow highlight around the card
        draw_rect(x - 3, y - 3, CARD_WIDTH + 6, CARD_HEIGHT + 6, 0xFFFF00, 0);
    }
    else
    {
        // Draw selection on grid for placement
        int x = GRID_LEFT_MARGIN + col * GRID_COL_WIDTH;
        int y = GRID_TOP_MARGIN + row * GRID_ROW_HEIGHT;

        // Draw a white semi-transparent rectangle to show valid placement
        draw_rect(x, y, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0x80FFFFFF, 0);

        // If we have a selected card, show the plant preview
        if (selected_card >= 0)
        {
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

void game_init()
{
    // Plant selection variables
    int selected_row = 0;
    int selected_col = 0;
    selection_mode = 0; // Start in card selection mode
    selected_card = -1; // No card selected initially
    // Draw initial selection (on first card)
    int current_selection = -1;
    // draw_selection(selected_row, selected_col);
    // draw_rect(CARD_START_X  ,CARD_START_Y , CARD_START_X +60, CARD_START_Y+75, 0xfF00ff, 0);
    // draw_plant(,selected_col, selected_row);
    draw_grid();
    while (1)
    {
        // update_zombie_position(&zombie1);
        // update_zombie_position(&zombie2);
        // update_zombie_position(&zombie3);
        // update_zombie_position(&zombie4);
        int x = 0;
        int y = 0;
        char key = getUart();
        switch (key)
        {
        case '0':
            current_selection = 0;
            break;
        case '1':
            current_selection = 1;
            break;
        case '2':
            current_selection = 2;
            break;
        case '3':
            current_selection = 3;
            break;
        case '4':
            current_selection = 4;
            break;
        case '5':
            current_selection = 5;
            break;
        case '6':
            current_selection = 6;
            break;
        default:
            break;
        }
        if (current_selection != -1)
        {
            grid_to_pixel(selected_col, selected_row, &x, &y);
        }

        if (key == '[')
        {
            char key2 = getUart();
            if (key2 == 'A')
            { // Up arrow
                // CARD_START_Y+= 90;
                if (selected_row <= 0)
                {
                    selected_row = 0;
                }
                else
                {
                    selected_row--;
                }

                if (current_selection != -1)
                {
                    restore_background_area(x, y, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0);

                    draw_plant(current_selection, selected_col, selected_row);
                }
            }
            else if (key2 == 'B')
            {
                if (selected_row >= 4)
                {
                    selected_row = 4;
                }
                else
                {
                    selected_row++;
                }
                if (current_selection != -1)
                {
                    restore_background_area(x, y, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0);
                    // grid_to_pixel(selected_col,selected_row, &x, &y );
                    draw_plant(current_selection, selected_col, selected_row);
                }
            }
            else if (key2 == 'C')
            {
                if (selected_col >= 9)
                {
                    selected_col = 9;
                }
                else
                {
                    selected_col++;
                }
                if (current_selection != -1)
                {
                    restore_background_area(x, y, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0);
                    // grid_to_pixel(selected_col,selected_row, x, y );
                    draw_plant(current_selection, selected_col, selected_row);
                }
            }
            else if (key2 == 'D')
            { // Left arrow
                if (selected_col <= 0)
                {
                    selected_col = 1;
                }
                else
                {
                    selected_col--;
                }
                if (current_selection != -1)
                {
                    restore_background_area(x, y, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0);
                    // grid_to_pixel(selected_col,selected_row, x, y );
                    draw_plant(current_selection, selected_col, selected_row);
                }
            }
            uart_puts("\n X:");
            uart_dec(x);
            uart_puts("\n Y :");
            uart_dec(y);
            uart_puts("\n");
        }

        if (key == '\n')
        {
            if (selection_mode == 0)
            {
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
            }
            else
            {
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

        if (key == '0')
        { // Deselect (cancel)
            // Return to card selection mode
            selection_mode = 0;
            selected_card = -1;
            selected_col = 0;
            draw_selection(selected_row, selected_col);
        }
        delay_ms(100);
    }
}

void start_level()
{
    // draw background first
    draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    draw_grid();

    // Define individual zombies instead of an array
    Zombie zombie1, zombie2, zombie3, zombie4, zombie5;
    Zombie zombie6, zombie7, zombie8, zombie9, zombie10;

    // Pointer array for zombies
    Zombie *zombie_pointers[10] = {
        &zombie1, &zombie2, &zombie3, &zombie4, &zombie5,
        &zombie6, &zombie7, &zombie8, &zombie9, &zombie10};

    int zombie_spawned[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int spawn_times[10] = {0, 100, 300, 450, 600, 750, 900, 1050, 1200, 1350};
    int zombie_types[10] = {
        ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_NORMAL,
        ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_BUCKET, ZOMBIE_HELMET};

    int zombie_rows[10] = {0, 1, 2, 3, 0, 1, 2, 3, 2, 1};

    int zombies_killed = 0;
    int frame_counter = 0;
    while (1)
    {
        set_wait_timer(1, 50);

        for (int i = 0; i < 10; i++)
        {
            if (frame_counter == spawn_times[i] && !zombie_spawned[i])
            {
                uart_puts("Spawning zombie ");
                uart_dec(i + 1);
                uart_puts(" of type ");
                uart_dec(zombie_types[i]);
                uart_puts(" at row ");
                uart_dec(zombie_rows[i]);
                uart_puts("\n");

                // Use temporary variable to hold the spawned zombie
                Zombie temp_zombie = spawn_zombie(zombie_types[i], zombie_rows[i]);

                // Then copy to the appropriate zombie
                switch (i)
                {
                case 0:
                    zombie1 = temp_zombie;
                    break;
                case 1:
                    zombie2 = temp_zombie;
                    break;
                case 2:
                    zombie3 = temp_zombie;
                    break;
                case 3:
                    zombie4 = temp_zombie;
                    break;
                case 4:
                    zombie5 = temp_zombie;
                    break;
                case 5:
                    zombie6 = temp_zombie;
                    break;
                case 6:
                    zombie7 = temp_zombie;
                    break;
                case 7:
                    zombie8 = temp_zombie;
                    break;
                case 8:
                    zombie9 = temp_zombie;
                    break;
                case 9:
                    zombie10 = temp_zombie;
                    break;
                }

                zombie_spawned[i] = 1;
            }
        }

        // Update all zombies using pointers
        for (int i = 0; i < 10; i++)
        {
            if (!zombie_spawned[i] || !zombie_pointers[i]->active)
                continue;

            // Print zombie position in per 30 frame counts
            if (frame_counter % 30 == 0)
            {
                uart_puts("Updating zombie ");
                uart_dec(i + 1);
                uart_puts(" at position x=");
                uart_dec(zombie_pointers[i]->x);
                uart_puts(", y=");
                uart_dec(zombie_pointers[i]->y);
                uart_puts("\n");
            }

            update_zombie_position(zombie_pointers[i]);

            // Check for game over
            if (zombie_pointers[i]->x <= 50)
            {
                game.state = GAME_OVER;
                uart_puts("Game Over - Zombie reached house\n");
                return;
            }

            // Check if killed
            if (zombie_pointers[i]->health <= 0)
            {
                zombie_pointers[i]->active = 0;
                zombies_killed++;
                game.score += ZOMBIE_KILL_REWARD;

                uart_puts("Kill Zombie + ");
                uart_dec(ZOMBIE_KILL_REWARD);
                uart_puts(" ,Total Score: ");
                uart_dec(game.score);
                uart_puts("\n");
            }

            // Check for level completion
            if (zombies_killed >= 10)
            {
                delay_ms(2000);

                game.state = GAME_VICTORY;
                return;
            }
        }
        frame_counter++;
        set_wait_timer(0, 0);
    }
}
