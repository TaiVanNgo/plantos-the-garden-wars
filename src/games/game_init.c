#include "../../include/game_init.h"

SelectionState select_state = {
    .mode = 0, .selected_card = -1, .row = 0, .col = 0, .current_plant = -1};

GameState game = {.state = GAME_MENU, .score = 0, .level = LEVEL_INTERMEDIATE_ENUM};

void game_main()
{

    while (1)
    {
        switch (game.state)
        {
        case GAME_MENU:
            game_menu();
            break;
        case GAME_PLAYING:
            start_level();
            break;
        case GAME_PAUSED:
            // Handle pause menu
            break;
        case GAME_OVER:
            // draw loose screen
            draw_image(LOSE_SCREEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
            break;
        case GAME_QUIT:
            break;
        default:
            break;
        }
    }
}

void game_menu()
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
                game.state = GAME_PLAYING;
                return;
            }
            else if (current_selection == 1)
            {
                uart_puts("Quit Game ");
                game.state = GAME_QUIT;
                return;
            }
        }
    }
}

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
    if (select_state.mode == 0)
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
        if (select_state.selected_card >= 0)
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
    prev_mode = select_state.mode;
}

void start_level()
{
    // draw background first
    draw_image(GARDEN, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    draw_grid();

    /* Plant Setting*/
    // Reset selection state to default value
    select_state.row = 0;
    select_state.col = 0;
    select_state.mode = 0;
    select_state.selected_card = -1;
    select_state.current_plant = -1;

    draw_grid();

    /* Zombie settings */
    // Define individual zombies instead of an array
    Zombie zombie1, zombie2, zombie3, zombie4, zombie5;
    Zombie zombie6, zombie7, zombie8, zombie9, zombie10;

    // Pointer array for zombies
    Zombie *zombie_pointers[10] = {
        &zombie1, &zombie2, &zombie3, &zombie4, &zombie5,
        &zombie6, &zombie7, &zombie8, &zombie9, &zombie10};

    int zombie_spawned[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int spawn_times[10] = {0, 100, 300, 450, 600, 750, 900, 1050, 1200, 1350};

    int zombie_types[10];
    set_zombie_types_level(game.level, zombie_types);

    int zombie_rows[10] = {0, 1, 2, 3, 0, 1, 2, 3, 2, 1};

    int zombies_killed = 0;
    int frame_counter = 0;
    while (1)
    {
        set_wait_timer(1, 50);
        /*====== USER LOGIC START ====== */
        if (uart_isReadByteReady())
        {
            handle_user_input(&frame_counter);
        }

        /*====== USER LOGIC END ====== */

        /*====== ZOMBIE LOGIC START ====== */
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
        /*====== ZOMBIE LOGIC END ====== */

        frame_counter++;
        set_wait_timer(0, 0);
    }
}

// Handle user input and return whether input was processed
int handle_user_input(int *frame_counter)
{
    char key = getUart();

    // Number keys for plant selection
    if (key >= '0' && key <= '6')
    {
        int selection = key - '0';
        handle_plant_selection(selection);
        return 1;
    }

    // Arrow keys
    if (key == '[')
    {
        handle_arrow_keys();
        return 1;
    }

    // Enter key (confirm selection/placement)
    if (key == '\n')
    {
        handle_enter_key();
        return 1;
    }

    return 0; // Key wasn't handled
}

// Handle plant selection with number keys
void handle_plant_selection(int plant_type)

{
    int x_card = 0, y_card = 0;
    select_state.current_plant = plant_type;

    if (select_state.current_plant != -1)
    {
        grid_to_pixel(select_state.col, select_state.row, &x_card, &y_card);
        restore_background_area(x_card, y_card, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0);
        draw_plant(select_state.current_plant, select_state.col, select_state.row);
    }
}

// Handle arrow key navigation
void handle_arrow_keys()
{
    int x_card = 0, y_card = 0;
    char key2 = getUart();

    // Get current position for potential restoration
    grid_to_pixel(select_state.col, select_state.row, &x_card, &y_card);

    // Process direction
    switch (key2)
    {
    case 'A': // Up arrow
        if (select_state.row > 0)
        {
            select_state.row--;
        }
        break;

    case 'B': // Down arrow
        if (select_state.row < 4)
        {
            select_state.row++;
        }
        break;

    case 'C': // Right arrow
        if (select_state.col < 9)
        {
            select_state.col++;
        }
        break;

    case 'D': // Left arrow
        if (select_state.col > 0)
        {
            select_state.col--;
        }
        break;

    default:
        return; // Unrecognized key, exit
    }

    // Update display if a plant is selected
    if (select_state.current_plant != -1)
    {
        restore_background_area(x_card, y_card, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0);
        draw_plant(select_state.current_plant, select_state.col, select_state.row);
    }

    // Debug output
    uart_puts("Card Position (");
    uart_dec(x_card);
    uart_puts(", ");
    uart_dec(y_card);
    uart_puts(")\n");
}

// Handle Enter key press
void handle_enter_key()
{
    if (select_state.mode == 0)
    {
        // Select this plant card
        select_state.selected_card = select_state.col;

        // Switch to grid placement mode
        select_state.mode = 1;
        select_state.row = 0;
        select_state.col = 0;
        draw_selection(select_state.row, select_state.col);
    }
    else
    {
        // Place the plant at the selected grid position
        // place_plant(selected_card, selected_row, selected_col);

        // Return to card selection mode
        select_state.mode = 0;
        select_state.selected_card = -1;
        select_state.col = 0;
        draw_selection(select_state.row, select_state.col);
    }
}

void set_zombie_types_level(int level, int zombie_types[10])
{
    if (level == LEVEL_EASY_ENUM)
    {
        for (int i = 0; i < 8; i++)
        {
            zombie_types[i] = ZOMBIE_NORMAL;
        }

        zombie_types[8] = ZOMBIE_BUCKET;
        zombie_types[9] = ZOMBIE_HELMET;
    }
    else if (level == LEVEL_INTERMEDIATE_ENUM)
    {
        // Intermediate level - 3 normal + 5 bucket + 2 helmet
        for (int i = 0; i < 3; i++)
        {
            zombie_types[i] = ZOMBIE_NORMAL;
        }
        for (int i = 3; i < 8; i++)
        {
            zombie_types[i] = ZOMBIE_BUCKET;
        }
        zombie_types[8] = ZOMBIE_HELMET;
        zombie_types[9] = ZOMBIE_HELMET;
    }
    else
    {
        // Hard level - mix of bucket and helmet zombies
        for (int i = 0; i < 5; i++)
        {
            zombie_types[i] = ZOMBIE_BUCKET;
        }
        for (int i = 5; i < 10; i++)
        {
            zombie_types[i] = ZOMBIE_HELMET;
        }
    }
}
