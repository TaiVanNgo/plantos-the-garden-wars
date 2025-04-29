#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "image.h"
#include "cli.h"




void main()
{
    // Set up the serial console
    uart_init();
    os_welcome(); // Display welcome message

    // Display initial prompt
    uart_puts("GROS> ");

    // Run CLI
    while (1)
    {   
        
        cli();
    }
}