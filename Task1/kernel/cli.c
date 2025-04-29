#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "cli.h"

// Custom implementation of strcmp
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}




// Command table
Command commands[] = {
    {"help", 
     "Show brief information of all commands", 
     "help - Show brief information of all commands\n"
     "help <command_name> - Show full information of a specific command\n"
     "Example: NexOS> help showinfo", // Updated to NexOS
     cmd_help},
    {"clear", 
     "Clear screen (scrolls down to current cursor position)", 
     "clear - Clear screen (scrolls down to current cursor position)\n"
     "Example: NexOS> clear", // Updated to NexOS
     cmd_clear},
    {"showinfo", 
     "Show board revision and MAC address", 
     "showinfo - Show board revision (value and information) and board MAC address in correct format\n"
     "Example: NexOS> showinfo", // Updated to NexOS
     cmd_showinfo},
    {"baudrate", 
     "Change the baudrate of current UART", 
     "baudrate - Allow the user to change the baudrate of current UART being used\n"
     "Supports baud rates: 9600, 19200, 38400, 57600, 115200 bits per second\n"
     "Example: NexOS> baudrate (not yet implemented)", // Updated to NexOS
     cmd_baudrate},
    {"handshake", 
     "Turn on/off CTS/RTS handshaking on current UART", 
     "handshake - Allow the user to turn on/off CTS/RTS handshaking on current UART if possible\n"
     "Example: NexOS> handshake (not yet implemented)", // Updated to NexOS
     cmd_handshake}
};

const int num_commands = sizeof(commands) / sizeof(commands[0]);



// Clear the current line on the terminal
void clear_line() {
    uart_puts("\r");
    for (int i = 0; i < MAX_CMD_SIZE + 7; i++) { // +7 for "NexOS> "
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

// Add a command to history
void add_to_history(char* command) {
    if (command[0] == '\0') return; // Don't store empty commands

    // Shift history if full
    if (history_count == MAX_HISTORY) {
        for (int i = 1; i < MAX_HISTORY; i++) {
            for (int j = 0; j < MAX_CMD_SIZE; j++) {
                history[i - 1][j] = history[i][j];
            }
        }
        history_count--;
    }

    // Add new command to history
    for (int i = 0; i < MAX_CMD_SIZE; i++) {
        history[history_count][i] = command[i];
        if (command[i] == '\0') break;
    }
    history_count++;
    history_pos = history_count; // Reset position to the end
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

    // Handle escape sequences for arrow keys
    if (c == 27) { // Escape character
        char next = uart_getc(); // Should be '['
        if (next == '[') {
            char arrow = uart_getc(); // Should be 'A' (up) or 'B' (down)
            if (arrow == 'A') { // Up arrow
                if (history_count > 0 && history_pos > 0) {
                    history_pos--;
                    // Copy history command to buffer
                    for (int i = 0; i < MAX_CMD_SIZE; i++) {
                        cli_buffer[i] = history[history_pos][i];
                        if (cli_buffer[i] == '\0') break;
                    }
                    index = 0;
                    while (cli_buffer[index] != '\0') index++;
                }
            } else if (arrow == 'B') { // Down arrow
                if (history_pos < history_count) {
                    history_pos++;
                    if (history_pos == history_count) {
                        // Clear buffer if at the end (new command)
                        cli_buffer[0] = '\0';
                        index = 0;
                    } else {
                        // Copy history command to buffer
                        for (int i = 0; i < MAX_CMD_SIZE; i++) {
                            cli_buffer[i] = history[history_pos][i];
                            if (cli_buffer[i] == '\0') break;
                        }
                        index = 0;
                        while (cli_buffer[index] != '\0') index++;
                    }
                }
            }
        }
    }
    // Handle backspace (ASCII 127)
    else if (c == 127) {
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

        // Add command to history
        add_to_history(cli_buffer);

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

