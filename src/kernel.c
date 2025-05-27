#include "../assets/backgrounds/background.h"
#include "../assets/backgrounds/garden.h"
#include "../include/bullet.h"
#include "../include/cmd.h"
#include "../include/framebf.h"
#include "../include/game_init.h"
#include "../include/mbox.h"
#include "../include/plants.h"
#include "../include/uart0.h"
#include "../include/uart1.h"
#include "../include/utils.h"
#include "../include/video.h"
#include "../include/zombies.h"

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
    // Initialize framebuffer
    framebf_init();
    // Display welcome message
    os_welcome();

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
#elif defined(TEAM_INIT)
void main()
{
    uart_init();
    framebf_init();
    wipe_transition();
    display_team_members(1);
}

#endif