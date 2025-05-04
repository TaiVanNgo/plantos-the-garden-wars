#include "../include/uart0.h"
#include "../include/uart1.h"
#include "../include/mbox.h"
#include "../include/framebf.h"
#include "../include/cmd.h"
#include "../include/video.h"
#include "../include/utils.h"
#include "../include/background.h"
// #define TASK1
#define TASK2_VID


unsigned int create_color(unsigned char r, unsigned char g, unsigned char b)
{
    return (r << 16) | (g << 8) | b;
}


void simple_wipe_transition(void)
{

    for (int y = 0; y < 600; y++) {
        draw_image(BACKGROUND, 0, 0, 984, y); 
        
        if (y >= 50 ) {
            draw_string(200, 20, "EMBEDDED SYSTEMS: OPERATING SYSTEMS AND INTERFACING", RED);
        }


        if (y >= 70) {
            draw_circle(160, 70, 8, GREEN_YELLOW, 1);
            draw_string(180, 65, "NGO VAN TAI (S3974892)", GREEN_YELLOW);
        }

        if (y >= 90 ) {
            draw_circle(160, 90, 8, BRIGHT_BLUE, 1);
            draw_string(180, 85, "HUYNH TAN PHAT (S3929218)", BRIGHT_BLUE);
        }

        if (y >= 110) {
            draw_circle(160, 110, 8, SALMON_RED, 1);
            draw_string(180, 105, "HUYNH THAI DUONG (S3978955)", SALMON_RED);
        }

        if (y >= 130) {
            draw_circle(160, 130, 8, THISTLE, 1);
            draw_string(180, 125, "NGUYEN PHAM ANH THU (S3926793)", THISTLE);
        }

        if (y >= 20) {
            int pulse = 0;
            int pulse_dir = 1;
            if (pulse_dir > 0) {
                pulse++;
                if (pulse >= 10) pulse_dir = -1;
            } else {
                pulse--;
                if (pulse <= 4) pulse_dir = 1;
            }

            draw_circle(170, 20, 15, PULSE_CIRCLE_COLOR, 1);
            draw_circle(170, 20, pulse, WHITE, 1);
            draw_circle(630, 20, 15, PULSE_CIRCLE_COLOR, 1);
            draw_circle(630, 20, pulse, WHITE, 1);
        }

        delay_ms(1); 
    }

    delay_ms(50);  
    clear_screen(); 
}



void display_team_members()
{
    draw_image(BACKGROUND, 0, 0, 984, 600);

    // TItle
    draw_string(200, 20, "EMBEDDED SYSTEMS: OPERATING SYSTEMS AND INTERFACING", RED);

    // Member 1
    draw_circle(160, 70, 8, GREEN_YELLOW, 1);
    draw_string(180, 65, "NGO VAN TAI (S3974892)", GREEN_YELLOW);

    // Member 2
    draw_circle(160, 90, 8, BRIGHT_BLUE, 1);
    draw_string(180, 85, "HUYNH TAN PHAT (S3929218)", BRIGHT_BLUE);

    // Member 3
    draw_circle(160, 110, 8, SALMON_RED, 1);
    draw_string(180, 105, "HUYNH THAI DUONG (S3978955)", SALMON_RED);

    // Member 4
    draw_circle(160, 130, 8, THISTLE, 1);
    draw_string(180, 125, "NGUYEN PHAM ANH THU (S3926793)", THISTLE);

    // Pulse circle animation
    int pulse = 0;
    int pulse_dir = 1;

    while (1)
    {
        // Pulse animation
        if (pulse_dir > 0)
        {
            pulse++;
            if (pulse >= 10)
                pulse_dir = -1;
        }
        else
        {
            pulse--;
            if (pulse <= 4)
                pulse_dir = 1;
        }
        // Clear previous left circle
        draw_circle(170, 20, 15, PULSE_CIRCLE_COLOR, 1);
        // Draw new pulse left circle
        draw_circle(170, 20, pulse, WHITE, 1);

        // Clear previous right circle
        draw_circle(630, 20, 15, PULSE_CIRCLE_COLOR, 1);
        // Draw new pulse right circle
        draw_circle(630, 20, pulse, WHITE, 1);

        // Process UART input
        char c = getUart();

        if (c == 'q')
        {
            uart_puts("Exit displaying team member!\n");
            clear_screen();
            return;
        }

        // Short delay for animation
        delay_ms(500);
    }
}

#ifdef TASK1
void main()
{
    // Set up the serial console
    uart_init();
    os_welcome(); // Display welcome message

    // Display initial prompt
    uart_puts("BitOS> ");

    // Run CLI
    while (1)
    {
        cli();
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
    play_video(&vid, 0, 50, vid.total_frames); 
    simple_wipe_transition();
    display_team_members();
    // Run CLI
    while (1)
    {
        char c = uart_getc();
        uart_sendc(c);
    }
}
#else // task 2 name
void main()
{

    uart_init();
    framebf_init();

    display_team_members();

    while (1)
    {
        char c = uart_getc();
        uart_sendc(c);
    }
}
#endif
