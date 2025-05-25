#include "../include/game_init.h"

#include "../include/bullet.h"
#include "../include/cooldown.h"

extern int flame_active[GRID_ROWS];
extern int flame_start_frames[GRID_ROWS];


SelectionState select_state = {
    .mode = 0, .selected_card = -1, .row = 0, .col = 0, .current_plant = -1};

GameState game = {.state = GAME_MENU, .score = 0, .level = LEVEL_HARD_ENUM, .sun_count = 2000};

Plant plant_grid[4][9];
int prev_col, prev_row;
void game_main() {
     game.state = GAME_MENU;
     
    while (1) {
        switch (game.state) {
            case GAME_MENU:
                game_menu();
                break;
            case GAME_PLAYING:
                start_level();
                break;
            case GAME_DIFFICULTY:
                game_start_difficulty();
                break;
            case GAME_PAUSED:
                // Handle pause menu
                break;
            case GAME_OVER:
                game_over();
                break;
            case GAME_VICTORY:
                victory_screen();
                break;
            case GAME_QUIT:
                return;
                break;
            default:
                break;
        }
    }
}

int check_occupied() {
    if (plant_grid[select_state.row][select_state.col].type != 255 &&
        plant_grid[select_state.row][select_state.col].type != -1) {
        uart_puts("Cell already occupied!\n");
        return 0;
    }
    return 1;
}

int check_clear() {
    if (plant_grid[prev_row][prev_col].type != 255 &&
        plant_grid[prev_row][prev_col].type != -1) {
        uart_puts("Cell already occupied!\n");
        return 1;
    }
    return 0;
}

void game_start_difficulty() {
    clear_screen();
    draw_image(MAIN_SCREEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);

    Button normal, medium, hard;
    button_init(&normal, 240, 300, 300, 130, NORMAL);
    button_init(&medium, 240, 400, 300, 130, MEDIUM);
    button_init(&hard, 240, 500, 300, 130, HARD);

    Button *buttons[3] = {&normal, &medium, &hard};
    int current_selection = 0;
    int previous_selection = current_selection;

    button_set_state(buttons[current_selection], BUTTON_SELECTED);
    button_draw_selection(buttons, current_selection, previous_selection, 0, 1, 150, 20);

    while (1) {
        char key = getUart();
        if (key == '[') {
            char key2 = getUart();
            if ((key2 == 'A')) {
                // 'up arrow' button
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);
                current_selection--;
                if (current_selection < 0) {
                    current_selection = 2;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection, 0, 1, 150, 20);
            } else if ((key2 == 'B')) {
                // 'down arrow' button
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);

                current_selection++;
                if (current_selection > 2) {
                    current_selection = 0;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection, 0, 1, 150, 20);
            }
        }

        if (key == '\n') {
            if (current_selection == 0) {
                clear_screen();
                game.state = GAME_PLAYING;
                game.level = LEVEL_EASY_ENUM;
                return;
            } else if (current_selection == 1) {
                clear_screen();
                game.state = GAME_PLAYING;
                game.level = LEVEL_MEDIUM_ENUM;
                return;
            } else if (current_selection == 2) {
                clear_screen();
                game.state = GAME_PLAYING;
                game.level = LEVEL_HARD_ENUM;
                return;
            }
        }
    }
}
void game_menu() {
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
    button_draw_selection(buttons, current_selection, previous_selection, 0, 1, 150, 20);

    while (1) {
        char key = getUart();
        if (key == '[') {
            char key2 = getUart();
            if ((key2 == 'A')) {
                // 'up arrow' button
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);
                current_selection--;
                if (current_selection < 0) {
                    current_selection = 1;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection, 0, 1, 150, 20);
            } else if ((key2 == 'B')) {
                // 'down arrow' button
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);

                current_selection++;
                if (current_selection > 1) {
                    current_selection = 0;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection, 0, 1, 150, 20);
            }
        }

        if (key == '\n') {
            if (current_selection == 0) {
                game.state = GAME_DIFFICULTY;
                return;
            } else if (current_selection == 1) {
                uart_puts("Quit Game ");
                game.state = GAME_QUIT;
                return;
            }
        }
    }
}

void start_level() {
    reset_zombie_counts();  // Reset zombie tracking

    // Reset flame effects
    for (int i = 0; i < GRID_ROWS; i++) {
        flame_active[i] = 0;
        flame_start_frames[i] = 0;
    }

    // draw background first
    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            plant_grid[i][j].type = -1;
        }
    }

    create_simulated_background(simulated_background, GARDEN, GARDEN_WIDTH, GARDEN_HEIGHT);
    create_simulated_background(tmp, GARDEN, GARDEN_WIDTH, GARDEN_HEIGHT);
    draw_image(simulated_background, 0, 0, GARDEN_WIDTH, GARDEN_HEIGHT, 0);
    draw_grid();
    draw_sun_count(game.sun_count);

    /* Plant Setting*/
    // Reset selection state to default value
    select_state.row = 0;
    select_state.col = 0;
    select_state.mode = 0;
    select_state.selected_card = -1;
    select_state.current_plant = -1;

    draw_grid();
    draw_cursor();

    // Initialize bullet system
    unsigned long freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    unsigned long start_counter;
    asm volatile("mrs %0, cntpct_el0" : "=r"(start_counter));
    unsigned long start_ms = start_counter * 1000 / freq;
    bullet_system_init(start_ms, 2000);  // Initialize with 1 second fire interval

    sun_system_init(start_ms);

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
    while (1) {
        set_wait_timer(1, 50);

        /*====== USER LOGIC START ====== */
        if (uart_isReadByteReady()) {
            handle_user_input(&frame_counter);
        }
        /*====== USER LOGIC END ====== */

        // Update cooldowns
        update_plant_cooldowns();

        // Update bullet system
        unsigned long current_counter;
        asm volatile("mrs %0, cntpct_el0" : "=r"(current_counter));
        unsigned long freq;
        asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
        unsigned long current_time_ms = current_counter * 1000 / freq;

        // Update bullets
        bullet_update(current_time_ms);
        bullet_draw();

        // Update suns
        update_suns(frame_counter);
        draw_suns();

        // Update flame effects with current frame
        update_flame_effects(frame_counter);

        // Draw cooldown overlays for all plant cards that are on cooldown
        draw_all_plant_cooldowns();

        /*====== ZOMBIE LOGIC START ====== */
        for (int i = 0; i < 10; i++) {
            if (frame_counter == spawn_times[i] && !zombie_spawned[i]) {
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
                switch (i) {
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
        for (int i = 0; i < 10; i++) {
            if (!zombie_spawned[i] || !zombie_pointers[i]->active)
                continue;

            // Print zombie position in per 100 frame counts
            if (frame_counter % 100 == 0) {
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

            // Check for chilli damage if flames are active on this row
            if (flame_active[zombie_pointers[i]->row]) {
                apply_chilli_damage(zombie_pointers[i]);
            }

            // Check for game over
            if (zombie_pointers[i]->x <= 50) {
                game.state = GAME_OVER;
                uart_puts("[Game State] Game Over - Zombie reached house\n");
                return;
            }

            // Check if killed
            if (zombie_pointers[i]->health <= 0 && !zombie_pointers[i]->active) {
                zombie_pointers[i]->active = 0;
                zombies_killed++;
                game.score += ZOMBIE_KILL_REWARD;

                uart_puts("[Plant] Kill Zombie + ");
                uart_dec(ZOMBIE_KILL_REWARD);
                uart_puts(" ,Total Score: ");
                uart_dec(game.score);
                uart_puts("\n");

            }

            // Check for level completion
            if (zombies_killed >= 10) {
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
int handle_user_input(int *frame_counter) {
    char key = getUart();

    // Number keys for plant selection
    if (key >= '1' && key <= '5') {
        int selection = key - '0';
        if(check_occupied()){
            clear_plant_from_background(select_state.col, select_state.row, 0, 0);
        }
        handle_plant_selection(selection);
        return 1;
    }

    // Arrow keys
    if (key == '[') {
        prev_col = select_state.col;
        prev_row = select_state.row;

        if (select_state.current_plant != -1) {
            uart_puts("selected _ plant\n");
            uart_dec(select_state.current_plant);
            uart_puts("\n");
        }

        handle_arrow_keys();
        return 1;
    }

    if (key == 'P' || key == 'p') {
        handle_remove_plant();
        return 1;  // Added return to indicate input was handled
    }

    // Deselect plant
    if (key == 'Q' || key == 'q') {
        // Reset selection state
        select_state.selected_card = -1;
        select_state.current_plant = -1;
        select_state.mode = 0;

        // Clear selection border
        draw_selection_border(-1);

        // Redraw cursor
        draw_cursor();
        return 1;
    }

    // Enter key (confirm selection/placement)
    if (key == '\n') {
        handle_enter_key(*frame_counter);
        return 1;
    }

    return 0;  // Key wasn't handled
}

void handle_remove_plant() {
    select_state.selected_card = 9;
    select_state.mode = 2;
    handle_plant_selection(9);
}

void handle_plant_selection(int plant_type) {
    // Check if plant is on cooldown
    if (plant_type >= 1 && plant_type <= 5) {
        if (is_plant_on_cooldown(plant_type)) {
            uart_puts("Plant is on cooldown! ");
            display_plant_cooldown(plant_type);
            return;
        }
        display_plant_cooldown(plant_type);
    }

    // Update selection state
    select_state.current_plant = plant_type;
    draw_selection_border(plant_type);

    // Handle shovel mode
    if (select_state.mode == 2) {
        // Draw shovel cursor at current position
        int x, y;
        grid_to_pixel(select_state.col, select_state.row, &x, &y);
        restore_background_area(x, y, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0, 0, 0, 0);
        draw_plant(SHOVEL, select_state.col, select_state.row);
        return;
    }

    // Handle plant preview
    if (select_state.current_plant != -1) {
        int x, y;
        grid_to_pixel(select_state.col, select_state.row, &x, &y);
        int taken = check_clear() ? 1 : 0;
        clear_plant_from_background(prev_col, prev_row, 0, taken);
        // clear_plant_from_background(select_state.col, select_state.row, 0, taken);
        place_plant_on_background(select_state.current_plant, select_state.col, select_state.row, simulated_background);
        restore_background_area(x, y, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0, 0, 0, 0);

        // Debug output
        uart_puts("Plant preview shown: ");
        uart_dec(select_state.current_plant);
        uart_puts(" at (");
        uart_dec(select_state.col);
        uart_puts(", ");
        uart_dec(select_state.row);
        uart_puts(")\n");
    }
}

void handle_arrow_keys() {
    int old_row = select_state.row;
    int old_col = select_state.col;
    char key2 = getUart();

    // Store previous position for background restoration
    prev_row = old_row;
    prev_col = old_col;

    // Process direction with proper boundary checks
    switch (key2) {
        case 'A':  // Up arrow
            if (select_state.row > 0) {
                select_state.row--;
            }
            break;

        case 'B':  // Down arrow
            if (select_state.row < GRID_ROWS - 1) {
                select_state.row++;
            }
            break;

        case 'C':  // Right arrow
            if (select_state.col < GRID_COLS - 1) {
                select_state.col++;
            }
            break;

        case 'D':  // Left arrow
            if (select_state.col > 0) {
                select_state.col--;
            }
            break;

        default:
            return;  // Unrecognized key, exit
    }

    // Only redraw if position changed
    if (old_row != select_state.row || old_col != select_state.col) {
        int x_old, y_old, x_new, y_new;

        // Get pixel coordinates for old and new positions
        grid_to_pixel(old_col, old_row, &x_old, &y_old);
        grid_to_pixel(select_state.col, select_state.row, &x_new, &y_new);

        // Restore old cell background first
        restore_background_area(x_old, y_old, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0, 0, 1, 0);

        // Automatically collect sun at the new position
        if (collect_sun_at_position(select_state.col, select_state.row)) {
            uart_puts("[Sun] Auto-collected sun at (");
            uart_dec(select_state.col);
            uart_puts(", ");
            uart_dec(select_state.row);
            uart_puts(")\n");
        }

        // Draw at new position
        if (select_state.current_plant != -1) {
            // Draw plant preview
            int taken = check_clear() ? 1 : 0;

            clear_plant_from_background(prev_col, prev_row, 0, taken);
            place_plant_on_background(select_state.current_plant, select_state.col, select_state.row, simulated_background);
        } else {
            // Draw regular cursor
            draw_image(cursor, x_new + 5, y_new + 7, CURSOR_WIDTH, CURSOR_HEIGHT, 0);
        }

        // Debug output
        uart_puts("Cursor moved to (");
        uart_dec(select_state.col);
        uart_puts(", ");
        uart_dec(select_state.row);
        uart_puts(")\n");
    }
}

void handle_enter_key(int frame_counter) {
    if (select_state.mode == 2) {
        // Shovel mode
        int x, y;
        plant_grid[select_state.row][select_state.col].type = 255;
        clear_plant_from_background(select_state.col, select_state.row, 0, 0);
        select_state.mode = 1;
        select_state.selected_card = -1;
        select_state.current_plant = -1;

        // clear selection border
        draw_selection_border(-1);

        // Draw cursor at new position
        draw_cursor();
        return;
    }

    // Check if the cell is already occupied
    if (!check_occupied()) {
        return;
    }

    // Check if a plant is selected
    if (select_state.current_plant == -1) {
        // No plant selected, just show cursor
        draw_cursor();
        return;
    }

    // Plant placement logic
    if (select_state.mode == 0 || select_state.mode == 1) {
        if (select_state.mode == 0) {
            // Start cooldown when plant is placed
            if (select_state.current_plant >= 1 && select_state.current_plant <= 5) {
                start_plant_cooldown(select_state.current_plant);
            }

            place_plant_on_background(select_state.current_plant, select_state.col, select_state.row, simulated_background);
            place_plant_on_background(select_state.current_plant, select_state.col, select_state.row, tmp);
            
            // Create and place the plant in one step
            Plant new_plant = create_plant(select_state.current_plant, select_state.col, select_state.row);
            plant_grid[select_state.row][select_state.col] = new_plant;

            // Register plant with bullet system if it's a shooting plant
            if (select_state.current_plant == PLANT_PEASHOOTER ||
                select_state.current_plant == PLANT_FROZEN_PEASHOOTER) {
                unsigned long current_counter;
                asm volatile("mrs %0, cntpct_el0" : "=r"(current_counter));
                unsigned long freq;
                asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
                unsigned long current_time_ms = current_counter * 1000 / freq;
                bullet_spawn_plant(select_state.col, select_state.row, current_time_ms, select_state.current_plant);
            } else if (select_state.current_plant == PLANT_SUNFLOWER) {
                // Register sunflower for sun generation
                register_sunflower(select_state.col, select_state.row, frame_counter);
            } else if (select_state.current_plant == PLANT_CHILLIES) {
                chillies_detonate(select_state.row, frame_counter);
                // Clear the chilli plant from the grid and background
                plant_grid[select_state.row][select_state.col].type = 255;
                clear_plant_from_background(select_state.col, select_state.row, 0, 0);
            }

            // Reset selection state
            select_state.selected_card = -1;
            select_state.current_plant = -1;
            select_state.mode = 1;

            // clear selection border
            draw_selection_border(-1);
        } else if (select_state.mode == 1) {
            // Start cooldown when plant is placed
            if (select_state.current_plant >= 1 && select_state.current_plant <= 5) {
                start_plant_cooldown(select_state.current_plant);
            }

            place_plant_on_background(select_state.current_plant, select_state.col, select_state.row, simulated_background);
            place_plant_on_background(select_state.current_plant, select_state.col, select_state.row, tmp);
            
            // Create and place the plant in one step
            Plant new_plant = create_plant(select_state.current_plant, select_state.col, select_state.row);
            plant_grid[select_state.row][select_state.col] = new_plant;

            // Register plant with bullet system if it's a shooting plant
            if (select_state.current_plant == PLANT_PEASHOOTER ||
                select_state.current_plant == PLANT_FROZEN_PEASHOOTER) {
                unsigned long current_counter;
                asm volatile("mrs %0, cntpct_el0" : "=r"(current_counter));
                unsigned long freq;
                asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
                unsigned long current_time_ms = current_counter * 1000 / freq;
                bullet_spawn_plant(select_state.col, select_state.row, current_time_ms, select_state.current_plant);
            } else if (select_state.current_plant == PLANT_CHILLIES) {
                chillies_detonate(select_state.row, frame_counter);
                plant_grid[select_state.row][select_state.col].type = 255;
                clear_plant_from_background(select_state.col, select_state.row, 0, 0);
            }

            select_state.mode = 0;
            select_state.selected_card = -1;
            select_state.current_plant = -1;

            draw_selection_border(-1);

            // Draw cursor at the new position
            draw_cursor();
        }
    }
}

void set_zombie_types_level(int level, int zombie_types[10]) {
    if (level == LEVEL_EASY_ENUM) {
        // easy level: 5 normal zombie + 3 bucket zombies + 2 helmet zombies
        for (int i = 0; i < 5; i++) {
            zombie_types[i] = ZOMBIE_NORMAL;
        }

        for (int i = 5; i < 8; i++) {
            zombie_types[i] = ZOMBIE_BUCKET;
        }

        zombie_types[8] = ZOMBIE_HELMET;
        zombie_types[9] = ZOMBIE_HELMET;
    } else if (level == LEVEL_MEDIUM_ENUM) {
        // Intermediate level - 3 normal + 3 bucket + 4 helmet
        for (int i = 0; i < 3; i++) {
            zombie_types[i] = ZOMBIE_NORMAL;
        }
        for (int i = 3; i < 6; i++) {
            zombie_types[i] = ZOMBIE_BUCKET;
        }
        for (int i = 6; i < 10; i++) {
            zombie_types[i] = ZOMBIE_HELMET;
        }
    } else {
        // Hard level- 4 buckets + 3 helmet + 3 footballs
        for (int i = 0; i < 4; i++) {
            zombie_types[i] = ZOMBIE_BUCKET;
        }
        for (int i = 4; i < 7; i++) {
            zombie_types[i] = ZOMBIE_HELMET;
        }
        for (int i = 7; i < 10; i++) {
            zombie_types[i] = ZOMBIE_FOOTBALL;
        }
    }
}

int get_selection_current_plant(void) {
    return select_state.current_plant;
}

int get_selection_row(void) {
    return select_state.row;
}

int get_selection_col(void) {
    return select_state.col;
}

void victory_screen() {
    clear_screen();
    draw_image(VICTORY_SCREEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);

    Button quit, start;
    button_init(&quit, 240, 300, 300, 130, QUIT);
    button_init(&start, 240, 450, 300, 130, START);

    Button *buttons[] = {&quit, &start};
    int current_selection = 0;
    int previous_selection = current_selection;

    button_set_state(buttons[current_selection], BUTTON_SELECTED);
    button_draw_selection(buttons, current_selection, previous_selection, 1, 1, 150, 20);

    while (1) {
        char key = getUart();
        if (key == '[') {
            char key2 = getUart();
            if ((key2 == 'A')) {
                // 'up arrow' button
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);
                current_selection--;
                if (current_selection < 0) {
                    current_selection = 1;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection, 1, 1, 150, 20);
            } else if ((key2 == 'B')) {
                // 'down arrow' button
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);

                current_selection++;
                if (current_selection > 1) {
                    current_selection = 0;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection, 1, 1, 150, 20);
            }
        }

        if (key == '\n') {
            if (current_selection == 0) {
                clear_screen();
                game.state = GAME_MENU;
                return;
            } else if (current_selection == 1) {
                clear_screen();
                game.state = GAME_PLAYING;
                return;
            }
        }
    }
}

void game_over() {
    // clear_screen();
    draw_image(LOSE_SCREEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);


    Button quit, retry;
    button_init(&quit, 100, 450, 300, 85, HOME);
    button_init(&retry, 470, 450, 300, 75, QUIT);

    Button *buttons[] = {&quit, &retry};
    int current_selection = 0;
    int previous_selection = current_selection;

    button_set_state(buttons[current_selection], BUTTON_SELECTED);
    button_draw_selection(buttons, current_selection, previous_selection, 0, 0, 90, 20);

    while (1) {
        char key = getUart();
        if (key == '[') {
            char key2 = getUart();
            if ((key2 == 'C')) {
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);
                current_selection--;
                if (current_selection < 0) {
                    current_selection = 1;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection, 0, 0, 90, 20);
            } else if ((key2 == 'D')) {
                // 'down arrow' button
                int previous_selection = current_selection;
                button_set_state(buttons[current_selection], BUTTON_NORMAL);

                current_selection++;
                if (current_selection > 1) {
                    current_selection = 0;
                }

                button_set_state(buttons[current_selection], BUTTON_SELECTED);
                button_draw_selection(buttons, current_selection, previous_selection, 0, 0, 90, 20);
            }
        }

        if (key == '\n') {
            if (current_selection == 0) {
                clear_screen();
                game.state = GAME_MENU;
                return;
            } else if (current_selection == 1) {
                clear_screen();
                game.state = GAME_PLAYING;
                return;
            }
        }
    }
}

// Function to draw the cursor at the current position
void draw_cursor() {
    int x, y;

    // Get pixel coordinates from grid position
    grid_to_pixel(select_state.col, select_state.row, &x, &y);

    // Restore background first to avoid artifacts
    restore_background_area(x, y, GRID_COL_WIDTH, GRID_ROW_HEIGHT, 0, 0, 0, 0);

    // If a plant is selected, draw the plant preview
    if (select_state.current_plant != -1 && select_state.current_plant != 9) {
        int taken = check_clear() ? 1 : 0;
        clear_plant_from_background(prev_col, prev_row, 0, taken);
        place_plant_on_background(select_state.current_plant, select_state.col, select_state.row, simulated_background);
    }
    // If shovel is selected
    else if (select_state.mode == 2) {
        draw_plant(SHOVEL, select_state.col, select_state.row);
    }
    // Otherwise draw normal cursor
    else {
        // Draw cursor in the top-left of the cell
        draw_image(cursor, x + 5, y + 7, CURSOR_WIDTH, CURSOR_HEIGHT, 0);
    }
}

// Draw cooldown overlays for all plant cards that are on cooldown
void draw_all_plant_cooldowns() {
    for (int i = 1; i <= 5; i++) {
        if (is_plant_on_cooldown(i)) {
            draw_plant_cooldown_text(i);
        }
    }
}