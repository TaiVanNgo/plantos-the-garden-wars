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
    switch (game.level)
    {
    case LEVEL_EASY_ENUM:
        start_level_easy();
        break;
    case LEVEL_INTERMEDIATE_ENUM:
        // start_level_intermediate();
        break;
    case LEVEL_HARD_ENUM:
        // start_level_hard();
        break;
    default:
        start_level_easy(); // Default for easy level
        break;
    }
}

void start_level_easy()
{
    // Draw background
    draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    draw_grid();

    // Array of 5 Zombies
    Zombie zombies[5];
    int zombie_spawned[5] = {0, 0, 0, 0, 0};
    int spawn_times[5] = {0, 300, 600, 900, 1200};
    int zombie_types[5] = {ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_NORMAL, ZOMBIE_BUCKET, ZOMBIE_HELMET};
    int zombie_rows[5] = {0, 1, 2, 3, 1};

    int zombies_killed = 0;
    int frame_counter = 0;

    zombies[0] = spawn_zombie(2, 3);
    uart_puts("Test");
    while (1)
    {
        set_wait_timer(1, 50);

        // Spawn zombies at the specific time
        for (int i = 0; i < 5; i++)
        {
            if (frame_counter == spawn_times[i] && !zombie_spawned[i])
            {
                uart_puts("Spawning zombie ");
                uart_dec(i);
                uart_puts(" of type ");
                uart_dec(zombie_types[i]);
                uart_puts(" at row ");
                uart_dec(zombie_rows[i]);
                uart_puts("\n");

                delay_ms(10);
                zombies[i] = spawn_zombie(zombie_types[i], zombie_rows[i]);
                zombie_spawned[i] = 1;
                delay_ms(10);
            }
        }

        for (int i = 0; i < 5; i++)
        {
            // update all zombies
            if (!zombie_spawned[i] || !zombies[i].active)
                continue;

            // Only print debug info every 30 frames to avoid UART overload
            if (frame_counter % 30 == 0)
            {
                uart_puts("Updating zombie ");
                uart_dec(i);
                uart_puts(" at position x=");
                uart_dec(zombies[i].x);
                uart_puts(", y=");
                uart_dec(zombies[i].y);
                uart_puts("\n");
            }

            // Update position
            update_zombie_position(&zombies[i]);

            // Check for game over
            if (zombies[i].x <= 50)
            {
                game.state = GAME_OVER;
                uart_puts("Game Over - Zombie reached house\n");
                return;
            }

            // Check if killed
            if (zombies[i].health <= 0)
            {
                zombies[i].active = 0;
                zombies_killed++;
                game.score += ZOMBIE_KILL_REWARD;

                uart_puts("Kill Zombie + ");
                uart_dec(ZOMBIE_KILL_REWARD);
                uart_puts(" ,Total Score: ");
                uart_dec(game.score);
            }

            if (zombies_killed >= 5)
            {
                draw_string(350, 300, "LEVEL COMPLETE!", GREEN, 2);
                delay_ms(2000);

                // Advance to next level
                game.level = LEVEL_INTERMEDIATE_ENUM;
                return;
            }
        }

        frame_counter++;
        set_wait_timer(0, 0);
    }
}

// void start_level_easy()
// {
//     // Draw background
//     draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
//     draw_grid();

//     // Create five zombies;
//     Zombie zombie1, zombie2, zombie3, zombie4, zombie5;
//     int zombie1_spawned = 0;
//     int zombie2_spawned = 0;
//     int zombie3_spawned = 0;
//     int zombie4_spawned = 0;
//     int zombie5_spawned = 0;

//     int zombies_killed = 0;
//     int frame_counter = 0;

//     while (1)
//     {
//         set_wait_timer(1, 50);

//         // Spawn zombies at specific times
//         if (frame_counter == 0 && !zombie1_spawned)
//         {
//             zombie1 = spawn_zombie(ZOMBIE_NORMAL, 0);
//             zombie1_spawned = 1;
//         }
//         if (frame_counter == 300 && !zombie2_spawned)
//         {
//             zombie2 = spawn_zombie(ZOMBIE_NORMAL, 1);
//             zombie2_spawned = 1;
//         }

//         if (frame_counter == 600 && !zombie3_spawned)
//         {
//             zombie3 = spawn_zombie(ZOMBIE_NORMAL, 2);
//             zombie3_spawned = 1;
//         }

//         if (frame_counter == 900 && !zombie4_spawned)
//         {
//             zombie4 = spawn_zombie(ZOMBIE_BUCKET, 3);
//             zombie4_spawned = 1;
//         }

//         if (frame_counter == 1200 && !zombie5_spawned)
//         {
//             zombie5 = spawn_zombie(ZOMBIE_HELMET, 1);
//             zombie5_spawned = 1;
//         }

//         // update zombie's position
//         if (zombie1_spawned && zombie1.active)
//         {
//             update_zombie_position(&zombie1);

//             // Check for zombie reaching house
//             if (zombie1.x <= 50)
//             {
//                 game.state = GAME_OVER;
//                 uart_puts("Game Over");
//                 return;
//             }

//             // Check if zombie was killed
//             if (zombie1.health <= 0)
//             {
//                 zombie1.active = 0;
//                 zombies_killed++;
//                 game.score += ZOMBIE_KILL_REWARD;
//             }
//         }

//         if (zombie2_spawned && zombie2.active)
//         {
//             update_zombie_position(&zombie2);

//             if (zombie2.x <= 50)
//             {
//                 game.state = GAME_OVER;
//                 return;
//             }

//             if (zombie2.health <= 0)
//             {
//                 zombie2.active = 0;
//                 zombies_killed++;
//                 game.score += ZOMBIE_KILL_REWARD;
//             }
//         }
//         if (zombie3_spawned && zombie3.active)
//         {
//             update_zombie_position(&zombie3);

//             if (zombie3.x <= 50)
//             {
//                 game.state = GAME_OVER;
//                 return;
//             }

//             if (zombie3.health <= 0)
//             {
//                 zombie3.active = 0;
//                 zombies_killed++;
//                 game.score += ZOMBIE_KILL_REWARD;
//             }
//         }

//         if (zombie4_spawned && zombie4.active)
//         {
//             update_zombie_position(&zombie4);

//             if (zombie4.x <= 50)
//             {
//                 game.state = GAME_OVER;
//                 return;
//             }

//             if (zombie4.health <= 0)
//             {
//                 zombie4.active = 0;
//                 zombies_killed++;
//                 game.score += ZOMBIE_KILL_REWARD;
//             }
//         }
//         if (zombie5_spawned && zombie5.active)
//         {
//             update_zombie_position(&zombie5);

//             if (zombie5.x <= 50)
//             {
//                 game.state = GAME_OVER;
//                 return;
//             }

//             if (zombie5.health <= 0)
//             {
//                 zombie5.active = 0;
//                 zombies_killed++;
//                 game.score += ZOMBIE_KILL_REWARD;
//             }
//         }
//         frame_counter++;
//         set_wait_timer(0, 0);
//     }
// }

// void start_level(GameState *game, LEVEL_DIFFICULTY difficulty)
// {
//     Level current_level;
//     switch (difficulty)
//     {
//     case LEVEL_EASY_ENUM:
//         current_level = LEVEL_EASY;
//         break;
//     case LEVEL_INTERMEDIATE_ENUM:
//         current_level = LEVEL_INTERMEDIATE;
//         break;
//     case LEVEL_HARD_ENUM:
//         // current_level = LEVEL_HARD;
//         break;
//     default:
//         break;
//     }

//     int zombies_spawned = 0;
//     int zombies_killed = 0;
//     int frame_counter = 0;

//     Zombie active_zombies[MAX_ZOMBIES_PER_LEVEL];
//     // memset(active_zombies, 0, sizeof(active_zombies));

//     uart_puts("\nLevel ");
//     uart_puts(difficulty == LEVEL_EASY_ENUM ? "Easy " : difficulty == LEVEL_INTERMEDIATE_ENUM ? "Intermediate "
//                                                                                               : "Hard ");
//     uart_puts("Start\n");

//     draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);

//     // play until all the zombies died
//     while (zombies_killed < current_level.zombie_count)
//     {
//         /* GET USER INPUT */

//         if (zombies_spawned < current_level.zombie_count && frame_counter >= current_level.spawn_times[zombies_spawned])
//         {
//             // spawn the next zombie if the
//             active_zombies[zombies_spawned] = spawn_zombie(
//                 current_level.zombie_types[zombies_spawned],
//                 current_level.zombie_rows[zombies_spawned]);

//             zombies_spawned++;
//         }

//         for (int i = 0; i < zombies_spawned; i++)
//         {
//             // Skip dead zombie
//             if (active_zombies[i].active == 0)
//                 continue;

//             update_zombie_position(&active_zombies[i]);
//             if (active_zombies[i].x <= 50)
//             {
//                 game->state = GAME_OVER;
//                 return;
//             }

//             // Check if zombie was killed by plants
//             if (active_zombies[i].health <= 0)
//             {
//                 active_zombies[i].active = 0;
//                 zombies_killed++;
//                 game->score += ZOMBIE_KILL_REWARD;
//             }

//             // Check collisions (Dev later)
//             // check_plant_zombie_collisions(&active_zombies[i]);
//         }

//         // Update all active plants (DEV LATER)
//         // update_plants();

//         // Update sun production and collection (DEV LATER)
//         // update_sun_resources();

//         // Display level information (DEV LATER, USING UART)
//         // char info[50];
//         // uartputs(info, "Level: %d  Score: %d  Lives: %d", game->level, game->score, game->lives);
//         frame_counter++;
//     }

//     // all zombies were killed, user won the level
//     game->state = GAME_VICTORY;
// }