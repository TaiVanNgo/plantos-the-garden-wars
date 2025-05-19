#include "../include/uart0.h"
#include "../include/uart1.h"
#include "../include/mbox.h"
#include "../include/framebf.h"
#include "../include/cmd.h"
#include "../include/video.h"
#include "../include/utils.h"
#include "../include/zombies.h"
#include "../include/plants.h"
#include "../assets/backgrounds/background.h"
#include "../assets/backgrounds/garden.h"
#include "games/bullet.h"
//#include "background.c"
//   #define TASK1
//  #define TASK2_VID
// #define ZOMBIE_INIT
// #define PLANT_INIT
  #define TASK3_BULLET

// void display_team_members(int show_bg)
// {
//     // if show background
//     if (show_bg)
//     {
//         draw_image(BACKGROUND, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
//     }

//     // Title
//     draw_string(260, 20, "EMBEDDED SYSTEMS", RED, 2);
//     draw_string(150, 50, "OPERATING SYSTEMS AND INTERFACING", RED, 2);

//     // Member 1

//     draw_circle(160, 80, 8, GREEN_YELLOW, 1);
//     draw_string(200, 75, "NGUYEN PHAM ANH THU (S3926793)", GREEN_YELLOW, 2);

//     // Member 2
//     draw_circle(160, 100, 8, BRIGHT_BLUE, 1);
//     draw_string(200, 95, "HUYNH TAN PHAT (S3929218)", BRIGHT_BLUE, 2);

//     // Member 3
//     draw_circle(160, 120, 8, SALMON_RED, 1);
//     draw_string(200, 115, "HUYNH THAI DUONG (S3978955)", SALMON_RED, 2);

//     // Member 4
//     draw_circle(160, 140, 8, THISTLE, 1);
//     draw_string(200, 135, "NGO VAN TAI (S3974892)", THISTLE, 2);

//     // Pulse circle animation
//     int pulse = 0;
//     int pulse_dir = 1;

//     while (1)
//     {
//         // Pulse animation
//         if (pulse_dir > 0)
//         {
//             pulse++;
//             if (pulse >= 10)
//                 pulse_dir = -1;
//         }
//         else
//         {
//             pulse--;
//             if (pulse <= 4)
//                 pulse_dir = 1;
//         }
//         // Clear previous left circle
//         draw_circle(210, 20, 15, PULSE_CIRCLE_COLOR, 1);
//         // Draw new pulse left circle
//         draw_circle(210, 20, pulse, WHITE, 1);

//         // Clear previous right circle
//         draw_circle(550, 20, 15, PULSE_CIRCLE_COLOR, 1);
//         // Draw new pulse right circle
//         draw_circle(550, 20, pulse, WHITE, 1);

//         // Process UART input
//         char c = getUart();

//         if (c == 'q')
//         {
//             uart_puts("Exit displaying team member!\n");
//             clear_screen();
//             return;
//         }

//         // Short delay for animation
//         delay_ms(500);
//     }
// }

#ifdef TASK1
void main()
{
    // Set up the serial console
    uart_init();
    uart_puts("Starting Bullet Game...\n");
    
    // Run the bullet game
    bullet_game();
    
    // After game ends, run CLI
    while (1)
    {
        char c = uart_getc();
        uart_sendc(c);
    }
}
#elif defined(TASK2_VID)
void main()
{
    // Set up the serial console
    uart_init();

    framebf_init();

    Video vid;
    video_init(&vid);
    play_video(&vid, 0, 0, vid.total_frames);
    wipe_transition();
    display_team_members(0);
    // Run CLI
    while (1)
    {
        char c = uart_getc();
        uart_sendc(c);
    }
}
#elif defined(ZOMBIE_INIT)
void main()
{
    uart_init();

    framebf_init();
    draw_image(GAME_BACKGROUND, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
    draw_zombie(100, 100);
     
    while (1)
    {
        char c = uart_getc();
        uart_sendc(c);
    }
}
#elif defined(PLANT_INIT)
void main()
{
    uart_init();
    framebf_init();
    
    // Draw the garden background
    draw_image(GARDEN, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
    
    draw_plant_grid();
    
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
    
    // After game ends, run CLI
    // while (1)
    // {
    //     char c = getUart();
    //     uart_dec(c);
    //     uart_sendc(c);
    // }
}
#else // task 2 name
void main()
{

    uart_init();
    framebf_init();

    display_team_members(1);

    while (1)
    {
        char c = uart_getc();
        uart_sendc(c);
    }
}
#endif
