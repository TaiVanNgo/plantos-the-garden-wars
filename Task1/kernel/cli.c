#include "cli.h"
#include "../uart/uart0.h"
#include "../uart/uart1.h"

void cli()
{
    static char cli_buffer[MAX_CMD_SIZE];
    static int index = 0;

    // Read character from UART
    char c = uart_getc();

    // Clear the current line by sending carriage return and clearing the line
    uart_puts("\r");
    for (int i = 0; i < MAX_CMD_SIZE + 6; i++) { // +6 for "GROS> "
        uart_sendc(' ');
    }
    uart_puts("\r");

    // Handle backspace (ASCII 127)
    if (c == 127)
    {
        if (index > 0)
        {
            index--; // Move index back
            cli_buffer[index] = '\0'; // Null-terminate at the new end
        }
    }
    // Handle new character input (normal characters)
    else if (c != '\n')
    {
        if (index < MAX_CMD_SIZE - 1)
        {
            cli_buffer[index] = c; // Store character into the buffer
            index++;
            cli_buffer[index] = '\0'; // Null-terminate the string
        }
    }
    // When newline is received, finalize the command
    else if (c == '\n')
    {
        uart_puts("\nGot commands: ");
        uart_puts(cli_buffer);
        uart_puts("\n");

        /* Compare with supported commands and execute
         * ........................................... */

        // Reset the buffer index for the next command
        index = 0;
        cli_buffer[0] = '\0'; // Clear the buffer
    }

    // Always display the prompt and the current buffer contents
    uart_puts("GROS> ");
    uart_puts(cli_buffer);
}

void os_welcome() {
    uart_puts("\n\n                     ___           ___           ___           ___           ___           ___                         ___                         ___           ___     \n"
              "     _____         /\\  \\         /\\  \\         /\\  \\         /\\__\\         /\\  \\         /\\__\\                       /\\  \\                       /\\  \\         /\\__\\    \n"
              "    /::\\  \\        \\:\\  \\       /::\\  \\        \\:\\  \\       /:/ _/_       |::\\  \\       /:/ _/_         ___         /::\\  \\                     /::\\  \\       /:/ _/_   \n"
              "   /:/\\:\\  \\        \\:\\  \\     /:/\\:\\  \\        \\:\\  \\     /:/ /\\  \\      |:|:\\  \\     /:/ /\\__\\       /\\__\\       /:/\\:\\  \\                   /:/\\:\\  \\     /:/ /\\  \\  \n"
              "  /:/  \\:\\__\\   ___  \\:\\  \\   /:/  \\:\\  \\   _____\\:\\  \\   /:/ /::\\  \\   __|:|\\:\\  \\   /:/ /:/ _/_     /:/  /      /:/ /::\\  \\   ___     ___   /:/  \\:\\  \\   /:/ /::\\  \\ \n"
              " /:/__/ \\:|__| /\\  \\  \\:\\__\\ /:/__/ \\:\\__\\ /::::::::\\__\\ /:/__\\/\\:\\__\\ /::::|_\\:\\__\\ /:/_/:/ /\\__\\   /:/__/      /:/_/:/\\:\\__\\ /\\  \\   /\\__\\ /:/__/ \\:\\__\\ /:/_/:/\\:\\__\\\n"
              " \\:\\  \\ /:/  / \\:\\  \\ /:/  / \\:\\  \\ /:/  / \\:\\~~\\~~\\/__/ \\:\\  \\ /:/  / \\:\\~~\\  \\/__/ \\:\\/:/ /:/  /  /::\\  \\      \\:\\/:/  \\/__/ \\:\\  \\ /:/  / \\:\\  \\ /:/  / \\:\\/:/ /:/  /\n"
              "  \\:\\  /:/  /   \\:\\  /:/  /   \\:\\  /:/  /   \\:\\  \\        \\:\\  /:/  /   \\:\\  \\        \\::/_/:/  /  /:/\\:\\  \\      \\::/__/       \\:\\  /:/  /   \\:\\  /:/  /   \\::/ /:/  / \n"
              "   \\:\\/:/  /     \\:\\/:/  /     \\:\\/:/  /     \\:\\  \\        \\:\\/:/  /     \\:\\  \\        \\:\\/:/  /   \\__/\\:\\  \\      \\:\\  \\        \\:\\/:/  /     \\:\\/:/  /     \\:/_/:/  /  \n"
              "    \\::/  /       \\::/  /       \\::/  /       \\:\\__\\        \\::/  /       \\:\\__\\        \\::/  /         \\:\\__\\      \\:\\__\\        \\::/  /       \\::/  /        /:/  /   \n"
              "     \\/__/         \\/__/         \\/__/         \\/__/         \\/__/         \\/__/         \\/__/           \\/__/       \\/__/         \\/__/         \\/__/         \\/__/     \n\n");

    uart_puts("Developed By Thai Duong, only Thai Duong - S39878955.\n\n");
}