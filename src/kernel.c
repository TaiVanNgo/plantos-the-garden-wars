#include "../include/uart0.h"
#include "../include/uart1.h"
#include "../include/mbox.h"
#include "../include/framebf.h"
#include "../include/cmd.h"
#include "../include/video.h"
#include "../include/utils.h"

// #define TASK1
#ifdef TASK1
void main()
{
    // Set up the serial console
    uart_init();
    os_welcome(); // Display welcome message

    // Display initial prompt
    uart_puts("BOMOS> ");

    // Run CLI
    while (1)
    {
        cli();
    }
}
#else // TASK 2
void main()
{
    // Set up the serial console
    uart_init();

    framebf_init();

    Video vid;
    video_init(&vid);

    play_video(&vid, 100, 100);

    // Test pattern - draw a red square
    const unsigned int *frame = video_get_current_frame(&vid);
    drawImage(frame, 100, 100, FRAME_WIDTH, FRAME_HEIGHT);

    // Run CLI
    while (1)
    {
        char c = uart_getc();
        uart_sendc(c);
    }
}
#endif
