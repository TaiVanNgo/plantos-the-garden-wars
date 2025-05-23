#include "../include/game_init.h"
#include "../include/bullet.h"

SelectionState select_state = {
    .mode = 0, .selected_card = -1, .row = 0, .col = 0, .current_plant = -1};

GameState game = {.state = GAME_MENU, .score = 0, .level = LEVEL_HARD_ENUM};

Plant plant_grid[GRID_ROWS][GRID_COLS];

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

int check_occupied()
{
    if (plant_grid[select_state.row][select_state.col].type != 255)
    {
        uart_puts("Cell already occupied!\n");
        return 0;
    }

    return 1;
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

void start_level()
{
    reset_zombie_counts(); // Reset zombie tracking

    // draw background first
    for (int i = 0; i < GRID_ROWS; i++)
    {
        for (int j = 0; j < GRID_COLS; j++)
        {
            plant_grid[i][j].type = -1;
        }
    }

    create_simulated_background(simulated_background, GARDEN, GARDEN_WIDTH, GARDEN_HEIGHT);
    draw_image(simulated_background, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    draw_grid();

    /* Plant Setting*/
    // Reset selection state to default value
    select_state.row = 0;
    select_state.col = 0;
    select_state.mode = 0;
    select_state.selected_card = -1;
    select_state.current_plant = -1;

    draw_grid();

    // Initialize bullet system
    unsigned long freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    unsigned long start_counter;
    asm volatile("mrs %0, cntpct_el0" : "=r"(start_counter));
    unsigned long start_ms = start_counter * 1000 / freq;
    bullet_system_init(start_ms, 1000); // Initialize with 1 second fire interval

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

        // Update bullet system
        unsigned long current_counter;
        asm volatile("mrs %0, cntpct_el0" : "=r"(current_counter));
        unsigned long current_time_ms = current_counter * 1000 / freq;

        // Update bullets
        bullet_update(current_time_ms);
        bullet_draw();

        /*====== ZOMBIE LOGIC START ====== */
        for (int i = 0; i < 10; i++)
        {
            if (frame_counter == spawn_times[i] && !zombie_spawned[i])
            {
                uart_puts("[Zombie] Spawning zombie ");
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

                register_zombie_on_row(zombie_rows[i], 1);
                zombie_spawned[i] = 1;
            }
        }

        // Update all zombies using pointers
        for (int i = 0; i < 10; i++)
        {
            if (!zombie_spawned[i] || !zombie_pointers[i]->active)
                continue;

            // Print zombie position in per 100 frame counts
            if (frame_counter % 100 == 0)
            {
                uart_puts("[Zombie] Updating zombie ");
                uart_dec(i + 1);
                uart_puts(" at position x=");
                uart_dec(zombie_pointers[i]->x);
                uart_puts(", y=");
                uart_dec(zombie_pointers[i]->y);
                uart_puts("\n");
            }

            update_zombie_position(zombie_pointers[i]);

            // Check for bullet collisions
            check_bullet_zombie_collisions(zombie_pointers[i]);

            // Check for game over
            if (zombie_pointers[i]->x <= 50)
            {
                game.state = GAME_OVER;
                uart_puts("[Game State] Game Over - Zombie reached house\n");
                return;
            }

            // Check if killed
            if (zombie_pointers[i]->health <= 0 && zombie_pointers[i]->active)
            {
                zombie_pointers[i]->active = 0;
                register_zombie_on_row(zombie_pointers[i]->row, 0);
                zombies_killed++;
                game.score += ZOMBIE_KILL_REWARD;

                uart_puts("[Plant] Kill Zombie + ");
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
    if (key >= '1' && key <= '5')
    {
        int selection = key - '0';
        handle_plant_selection(selection);
        return 1;
    }

    // Arrow keys
    if (key == '[' && select_state.current_plant != -1)
    {
        uart_puts("selected _ plant\n");
        uart_dec(select_state.current_plant);
        uart_puts("\n");
        handle_arrow_keys();
        return 1;
    }

    if (key == 'P' || key == 'p')
    {
        handle_remove_plant();
    }
    // Enter key (confirm selection/placement)
    if (key == '\n')
    {
        handle_enter_key();
        return 1;
    }

    return 0; // Key wasn't handled
}

void handle_remove_plant()
{
    select_state.selected_card = 9;
    select_state.mode = 2;
    handle_plant_selection(9);
}
// Handle plant selection with number keys
void handle_plant_selection(int plant_type)
{
    int x_card = 0, y_card = 0;
    select_state.current_plant = plant_type;

    draw_selection_border(plant_type);
    if (select_state.mode == 2)
    {
        grid_to_pixel(select_state.col, select_state.row, &x_card, &y_card);
        restore_background_area(x_card, y_card, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0, 0);
        draw_plant(select_state.current_plant, select_state.col, select_state.row);
        return;
    }

    if (select_state.current_plant != -1)
    {
        grid_to_pixel(select_state.col, select_state.row, &x_card, &y_card);
        restore_background_area(x_card, y_card, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0, 0);
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
        restore_background_area(x_card, y_card, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0, 0);
        draw_plant(select_state.current_plant, select_state.col, select_state.row);
    }

    // Debug output
    uart_puts("Card Position (");
    uart_dec(x_card);
    uart_puts(", ");
    uart_dec(y_card);
    uart_puts(")\n");
}

void handle_enter_key()
{
    if (select_state.mode == 2)
    {
        int x, y;
        plant_grid[select_state.row][select_state.col].type = 255;
        clear_plant_from_background(select_state.col, select_state.row);
        select_state.mode = 1;
        select_state.selected_card = -1;
        select_state.current_plant = -1;

        // clear selection border
        draw_selection_border(-1);

        select_state.row = 0;
        select_state.col = 0;
        return;
    }

    if (!check_occupied())
    {
        return;
    }
    if (select_state.mode == 0)
    {
        place_plant_on_background(select_state.current_plant, select_state.col, select_state.row, simulated_background);
        Plant new_plant = create_plant(select_state.current_plant, select_state.col, select_state.row);
        plant_grid[select_state.row][select_state.col] = new_plant;

        // Register plant with bullet system if it's a shooting plant
        if (select_state.current_plant == PLANT_PEASHOOTER ||
            select_state.current_plant == PLANT_FROZEN_PEASHOOTER)
        {
            unsigned long current_counter;
            asm volatile("mrs %0, cntpct_el0" : "=r"(current_counter));
            unsigned long freq;
            asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
            unsigned long current_time_ms = current_counter * 1000 / freq;
            bullet_spawn_plant(select_state.col, select_state.row, current_time_ms);
        }

        select_state.selected_card = -1;
        select_state.current_plant = -1;

        // clear selection border
        draw_selection_border(-1);

        select_state.mode = 1;
        select_state.row = 0;
        select_state.col = 0;
    }
    else if (select_state.mode == 1)
    {
        place_plant_on_background(select_state.current_plant, select_state.col, select_state.row, simulated_background);
        Plant new_plant = create_plant(select_state.current_plant, select_state.col, select_state.row);
        plant_grid[select_state.row][select_state.col] = new_plant;

        // Register plant with bullet system if it's a shooting plant
        if (select_state.current_plant == PLANT_PEASHOOTER ||
            select_state.current_plant == PLANT_FROZEN_PEASHOOTER)
        {
            unsigned long current_counter;
            asm volatile("mrs %0, cntpct_el0" : "=r"(current_counter));
            unsigned long freq;
            asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
            unsigned long current_time_ms = current_counter * 1000 / freq;
            bullet_spawn_plant(select_state.col, select_state.row, current_time_ms);
        }

        select_state.mode = 0;
        select_state.selected_card = -1;
        select_state.current_plant = -1;

        draw_selection_border(-1);

        select_state.row = 0;
        select_state.col = 0;
    }
}

void set_zombie_types_level(int level, int zombie_types[10])
{
    if (level == LEVEL_EASY_ENUM)
    {
        // easy level: 5 normal zombie + 3 bucket zombies + 2 helmet zombies
        for (int i = 0; i < 5; i++)
        {
            zombie_types[i] = ZOMBIE_NORMAL;
        }

        for (int i = 5; i < 8; i++)
        {
            zombie_types[i] = ZOMBIE_BUCKET;
        }

        zombie_types[8] = ZOMBIE_HELMET;
        zombie_types[9] = ZOMBIE_HELMET;
    }
    else if (level == LEVEL_INTERMEDIATE_ENUM)
    {
        // Intermediate level - 3 normal + 3 bucket + 4 helmet
        for (int i = 0; i < 3; i++)
    {
            zombie_types[i] = ZOMBIE_NORMAL;
        }
        for (int i = 3; i < 6; i++)
        {
            zombie_types[i] = ZOMBIE_BUCKET;
        }
        for (int i = 6; i < 10; i++)
        {
            zombie_types[i] = ZOMBIE_HELMET;
        }
    }
    else
    {
        // Hard level- 4 buckets + 3 helmet + 3 footballs
        for (int i = 0; i < 4; i++)
        {
            zombie_types[i] = ZOMBIE_BUCKET;
        }
        for (int i = 4; i < 7; i++)
        {
            zombie_types[i] = ZOMBIE_HELMET;
        }
        for (int i = 7; i < 10; i++)
        {
            zombie_types[i] = ZOMBIE_FOOTBALL;
        }
    }
}

int get_selection_current_plant(void)
{
    return select_state.current_plant;
}

int get_selection_row(void)
{
    return select_state.row;
}

int get_selection_col(void)
{
    return select_state.col;
}