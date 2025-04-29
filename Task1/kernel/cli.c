#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"

#include "cli.h"

// Define NULL for our use
#ifndef NULL
#define NULL ((void *)0)
#endif

// Custom implementation of strcmp
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}


// Forward declarations of command functions
void cmd_help(char* args);
void cmd_clear(char* args);
void cmd_showinfo(char* args);
void cmd_baudrate(char* args);
void cmd_handshake(char* args);

// Command table
Command commands[] = {
    {"help", 
     "Show brief information of all commands", 
     "help - Show brief information of all commands\n"
     "help <command_name> - Show full information of a specific command\n"
     "Example: GROS> help showinfo",
     cmd_help},
    {"clear", 
     "Clear screen (scrolls down to current cursor position)", 
     "clear - Clear screen (scrolls down to current cursor position)\n"
     "Example: GROS> clear",
     cmd_clear},
    {"showinfo", 
     "Show board revision and MAC address", 
     "showinfo - Show board revision (value and information) and board MAC address in correct format\n"
     "Example: GROS> showinfo",
     cmd_showinfo},
    {"baudrate", 
     "Change the baudrate of current UART", 
     "baudrate - Allow the user to change the baudrate of current UART being used\n"
     "Supports baud rates: 9600, 19200, 38400, 57600, 115200 bits per second\n"
     "Example: GROS> baudrate (not yet implemented)",
     cmd_baudrate},
    {"handshake", 
     "Turn on/off CTS/RTS handshaking on current UART", 
     "handshake - Allow the user to turn on/off CTS/RTS handshaking on current UART if possible\n"
     "Example: GROS> handshake (not yet implemented)",
     cmd_handshake}
};

const int num_commands = sizeof(commands) / sizeof(commands[0]);

// Clear the current line on the terminal
void clear_line() {
    uart_puts("\r");
    for (int i = 0; i < MAX_CMD_SIZE + 6; i++) { // +6 for "GROS> "
        uart_sendc(' ');
    }
    uart_puts("\r");
}

// Display the prompt and current buffer contents
void display_prompt(char* buffer) {
    uart_puts(PROMPT);
    uart_puts(buffer);
}

// Extract the command name and arguments from the buffer
void parse_command(char* buffer, char** cmd_name, char** args) {
    *cmd_name = buffer;
    *args = buffer;

    // Find the first space to separate command name and arguments
    while (**args != ' ' && **args != '\0') {
        (*args)++;
    }

    if (**args == ' ') {
        **args = '\0'; // Null-terminate the command name
        (*args)++;
        // Skip any additional spaces
        while (**args == ' ') (*args)++;
    }

    if (**args == '\0') {
        *args = NULL; // No arguments
    }
}

// Command implementations
void cmd_help(char* args) {
    if (args == NULL) {
        // Show brief information for all commands
        uart_puts("\nAvailable commands:\n");
        for (int i = 0; i < num_commands; i++) {
            uart_puts(commands[i].name);
            uart_puts(" - ");
            uart_puts(commands[i].brief);
            uart_puts("\n");
        }
    } else {
        // Show detailed information for a specific command
        int found = 0;
        for (int i = 0; i < num_commands; i++) {
            if (strcmp(args, commands[i].name) == 0) {
                uart_puts("\n");
                uart_puts(commands[i].details);
                uart_puts("\n");
                found = 1;
                break;
            }
        }
        if (!found) {
            uart_puts("\nCommand not found: ");
            uart_puts(args);
            uart_puts("\n");
        }
    }
}

void cmd_clear(char* args) {
    // Scroll down to "clear" the screen by printing newlines
    for (int i = 0; i < 30; i++) { // Adjust the number of lines as needed
        uart_puts("\n");
    }
}

void cmd_showinfo(char* args) {
    uart_puts("\nshowinfo: Not yet implemented.\n");
}

void cmd_baudrate(char* args) {
    uart_puts("\nbaudrate: Not yet implemented.\n");
}

void cmd_handshake(char* args) {
    uart_puts("\nhandshake: Not yet implemented.\n");
}

void cli() {
    static char cli_buffer[MAX_CMD_SIZE];
    static int index = 0;

    // Read character from UART
    char c = uart_getc();

    // Clear the current line
    clear_line();

    // Handle backspace (ASCII 127)
    if (c == 127) {
        if (index > 0) {
            index--; // Move index back
            cli_buffer[index] = '\0'; // Null-terminate at the new end
        }
    }
    // Handle new character input (normal characters)
    else if (c != '\n') {
        if (index < MAX_CMD_SIZE - 1) {
            cli_buffer[index] = c; // Store character into the buffer
            index++;
            cli_buffer[index] = '\0'; // Null-terminate the string
        }
    }
    // When newline is received, process the command
    else if (c == '\n') {
        uart_puts("\nGot command: ");
        uart_puts(cli_buffer);
        uart_puts("\n");

        // Parse the command and arguments
        char* cmd_name;
        char* args;
        parse_command(cli_buffer, &cmd_name, &args);

        // Execute the command
        int found = 0;
        for (int i = 0; i < num_commands; i++) {
            if (strcmp(cmd_name, commands[i].name) == 0) {
                commands[i].execute(args);
                found = 1;
                break;
            }
        }
        if (!found && cli_buffer[0] != '\0') {
            uart_puts("Unknown command: ");
            uart_puts(cli_buffer);
            uart_puts("\n");
        }

        // Reset the buffer for the next command
        index = 0;
        cli_buffer[0] = '\0';
    }

    // Display the prompt and current buffer contents
    display_prompt(cli_buffer);
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