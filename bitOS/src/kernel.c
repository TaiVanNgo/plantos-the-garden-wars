#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "../include/mbox.h"
#include "../include/framebf.h"
#include "../include/cmd.h"

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