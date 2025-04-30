#include "../include/uart0.h"
#include "../include/uart1.h"
#include "../include/mbox.h"
#include "../include/framebf.h"
#include "../include/video.h"

void delay_ms(unsigned int ms)
{
    // Each iteration takes about 4 cycles
    // ARM CPU frequency is typically 1GHz = 1,000,000,000 cycles per second
    // So for 1ms we need 250,000 iterations (1,000,000,000 / (4 * 1000))
    unsigned int count = ms * 250000;
    for (unsigned int i = 0; i < count; i++)
    {
        asm volatile("nop"); // No operation, prevents optimization
    }
}

void main()
{
    // set up serial console
    uart_init();

    // say hello
    uart_puts("\n\nHello World!\n");

    // Initialize frame buffer
    framebf_init();

    Video vid;
    video_init(&vid);

    play_video(&vid, 100, 100);

    // echo everything back
    while (1)
    {
        // read each char
        char c = uart_getc();

        // send back
        uart_sendc(c);
    }
}