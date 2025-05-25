#include "../include/uart0.h"
#include "../include/uart1.h"
#include "../include/mbox.h"
#include "../include/framebf.h"
#include "../include/cmd.h"
#include "../include/video.h"
#include "../include/utils.h"
#include "../include/zombies.h"
#include "../include/plants.h"
#include "../include/bullet.h"
#include "../include/game_init.h"
#include "../assets/backgrounds/background.h"
#include "../assets/backgrounds/garden.h"

#if defined(ALL_INIT)
void main()
{
    // Initialize UART and framebuffer
    uart_init();
    framebf_init();

    // First play the video
    uart_puts("Playing intro video...\n");
    Video vid;
    video_init(&vid);
    play_video(&vid, 80, 120, vid.total_frames);

    // Then show team members
    wipe_transition();
    uart_puts("Displaying team members...\n");
    display_team_members(1);

    // Finally start CLI
    uart_puts("Starting CLI...\n");
    os_welcome();
    uart_puts(PROMPT);

    // Main CLI loop
    while (1)
    {
        cli();
    }
}
#elif defined(CLI_INIT)
void main()
{
    // Initialize UART for CLI
    uart_init();

    // Display welcome message
    os_welcome();

    // Initialize framebuffer
    framebf_init();

    // Initialize game components
    // game_init();

    // Display initial prompt
    uart_puts(PROMPT);

    // Main loop - handle both CLI and game input
    while (1)
    {
        // Process CLI commands
        cli();
    }
}
#elif defined(VID_INIT)
void main()
{
    // Set up the serial console
    uart_init();

    framebf_init();
    uart_puts("TASK2_VID main running\n");

    Video vid;
    video_init(&vid);
    play_video(&vid, 80, 120, vid.total_frames);
    wipe_transition();
    display_team_members(0);

    // Run CLI
    while (1)
    {
        char c = uart_getc();
        uart_sendc(c);

        if (c == 'c')
        {
            play_video(&vid, 80, 120, vid.total_frames);
        }
    }
}
#elif defined(GAME_INIT)
void main()
{
    uart_init();
    framebf_init();
    game_main();

    while (1)
    {
        char c = uart_getc();
        uart_dec(c);
    }
}
#elif defined(PLANT_INIT)
void main()
{
    uart_init();
    framebf_init();

    // Draw the garden background
    draw_image(GARDEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);

    draw_grid();

    fill_plant_grid();

    // Main loop
    while (1)
    {
        char c = getUart();
        uart_sendc(c);
    }
}
#elif defined(TASK3_BULLET)
void main()
{
    // Set up the serial console
    uart_init();
    uart_puts("Starting Bullet Game...\n");

    // Run the bullet game
    bullet_game();

    // start_game();

    // After game ends, run CLI
    while (1)
    {
        char c = getUart();
        uart_dec(c);
        uart_sendc(c);
    }
}
#elif defined(TEAM_INIT)
void main()
{

    uart_init();
    framebf_init();

    draw_image(QUIT, 0, 0, 300, 85, 0);
    wipe_transition();
    display_team_members(1);
}

#endif