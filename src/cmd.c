/**
 * @file cli.c
 * @brief Command Line Interface (CLI) implementation for BitOS.
 *
 * This file implements the CLI for BitOS, a lightweight operating system designed
 * for embedded systems (e.g., Raspberry Pi). The CLI allows users to interact with
 * the system via UART by entering commands such as "help", "clear", etc.
 * Features include command parsing, history navigation (up/down arrow keys),
 * and a welcome banner with animation.
 *
 * @author Thai Duong (S39878955)
 */

#include "../include/cmd.h"
#include "utils.h"

/**
 * @brief Simple busy-wait delay function.
 *
 * @param milliseconds Approximate delay in milliseconds (needs tuning for accuracy).
 * @note This is a rough approximation and may need adjustment based on system clock speed.
 */
void delay(int milliseconds)
{
    volatile unsigned int count = milliseconds * 100000; // Example: 100,000 cycles per ms
    while (count--)
    {
        asm("nop"); // No operation to keep the loop busy
    }
}


// === Command Definitions ===

/**
 * @brief Table of supported CLI commands.
 *
 * Each entry defines a command with its name, brief description, detailed help text,
 * and function pointer to its implementation.
 */
Command commands[] = {
    {"help",
     "Show brief information of all commands",
     "help - Show brief information of all commands\n"
     "help <command_name> - Show full information of a specific command\n"
     "Example: BitOS> help showinfo",
     cmd_help},
    {"clear",
     "Clear screen (scrolls down to current cursor position)",
     "clear - Clear screen (scrolls down to current cursor position)\n"
     "Example: BitOS> clear",
     cmd_clear},
    {"showinfo",
     "Show board revision and MAC address",
     "showinfo - Show board revision (value and information) and board MAC address in correct format\n"
     "Example: BitOS> showinfo",
     cmd_showinfo},
    {"baudrate",
     "Change the baudrate of current UART",
     "baudrate - Allow the user to change the baudrate of current UART being used\n"
     "Supports baud rates: 9600, 19200, 38400, 57600, 115200 bits per second\n"
     "Example: BitOS> baudrate (not yet implemented)",
     cmd_baudrate},
    {"handshake",
     "Turn on/off CTS/RTS handshaking on current UART",
     "handshake - Allow the user to turn on/off CTS/RTS handshaking on current UART if possible\n"
     "Example: BitOS> handshake (not yet implemented)",
     cmd_handshake}};

const int num_commands = sizeof(commands) / sizeof(commands[0]);

// === Command History Management ===

static char history[MAX_HISTORY][MAX_CMD_SIZE]; // Command history buffer
static int history_count = 0;                   // Number of commands in history
static int history_pos = 0;                     // Current position in history for navigation

/**
 * @brief Add a command to the history buffer.
 *
 * @param command The command string to add.
 * @note Empty commands are not stored. If history is full, the oldest command is removed.
 */
void add_to_history(char *command)
{
    if (command[0] == '\0')
        return; // Don't store empty commands

    // Shift history if full
    if (history_count == MAX_HISTORY)
    {
        for (int i = 1; i < MAX_HISTORY; i++)
        {
            for (int j = 0; j < MAX_CMD_SIZE; j++)
            {
                history[i - 1][j] = history[i][j];
            }
        }
        history_count--;
    }

    // Add new command to history
    for (int i = 0; i < MAX_CMD_SIZE; i++)
    {
        history[history_count][i] = command[i];
        if (command[i] == '\0')
            break;
    }
    history_count++;
    history_pos = history_count; // Reset position to the end
}

// === CLI Core Functions ===

/**
 * @brief Clear the current line on the terminal.
 *
 * Overwrites the line with spaces and returns the cursor to the start.
 */
void clear_line()
{
    uart_puts("\r");
    for (int i = 0; i < MAX_CMD_SIZE + 7; i++)
    { // +7 for "BitOS> "
        uart_sendc(' ');
    }
    uart_puts("\r");
}

/**
 * @brief Display the CLI prompt and current buffer contents.
 *
 * @param buffer The current command buffer to display.
 */
void display_prompt(char *buffer)
{
    uart_puts(PROMPT);
    uart_puts(buffer);
}

/**
 * @brief Parse a command string into command name and arguments.
 *
 * @param buffer The command buffer to parse.
 * @param cmd_name Pointer to store the command name (points within buffer).
 * @param args Pointer to store the arguments (or NULL if none).
 */
void parse_command(char *buffer, char **cmd_name, char **args)
{
    *cmd_name = buffer;
    *args = buffer;

    // Find the first space to separate command name and arguments
    while (**args != ' ' && **args != '\0')
    {
        (*args)++;
    }

    if (**args == ' ')
    {
        **args = '\0'; // Null-terminate the command name
        (*args)++;
        // Skip any additional spaces
        while (**args == ' ')
            (*args)++;
    }

    if (**args == '\0')
    {
        *args = NULL; // No arguments
    }
}

/**
 * @brief Main CLI loop to read and process user input.
 *
 * Reads characters from UART, handles command input, history navigation,
 * and executes commands when Enter is pressed.
 */
void cli()
{
    static char cli_buffer[MAX_CMD_SIZE]; // Current command buffer
    static int index = 0;                 // Current position in the buffer

    // Read character from UART
    char c = uart_getc();

    // Handle TAB key for autocompletion (ASCII 9)
    if (c == 9)
    {
        int needs_update = handle_tab_completion(cli_buffer, &index);
        if (needs_update)
        {
            clear_line();
            display_prompt(cli_buffer);
        }
        return;
    }

    // Clear the current line
    clear_line();

    // Handle escape sequences for arrow keys
    if (c == 27)
    {                            // Escape character (start of arrow key sequence)
        char next = uart_getc(); // Should be '['
        if (next == '[')
        {
            char arrow = uart_getc(); // Should be 'A' (up) or 'B' (down)
            if (arrow == 'A')
            { // Up arrow
                if (history_count > 0 && history_pos > 0)
                {
                    history_pos--;
                    // Copy history command to buffer
                    for (int i = 0; i < MAX_CMD_SIZE; i++)
                    {
                        cli_buffer[i] = history[history_pos][i];
                        if (cli_buffer[i] == '\0')
                            break;
                    }
                    index = 0;
                    while (cli_buffer[index] != '\0')
                        index++;
                }
            }
            else if (arrow == 'B')
            { // Down arrow
                if (history_pos < history_count)
                {
                    history_pos++;
                    if (history_pos == history_count)
                    {
                        // Clear buffer if at the end (new command)
                        cli_buffer[0] = '\0';
                        index = 0;
                    }
                    else
                    {
                        // Copy history command to buffer
                        for (int i = 0; i < MAX_CMD_SIZE; i++)
                        {
                            cli_buffer[i] = history[history_pos][i];
                            if (cli_buffer[i] == '\0')
                                break;
                        }
                        index = 0;
                        while (cli_buffer[index] != '\0')
                            index++;
                    }
                }
            }
        }
    }
    // Handle backspace (ASCII 127)
    else if (c == 127)
    {
        if (index > 0)
        {
            index--;                  // Move index back
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
    // When newline is received, process the command
    else if (c == '\n')
    {
        uart_puts("\nGot command: ");
        uart_puts(cli_buffer);
        uart_puts("\n");

        // Add command to history
        add_to_history(cli_buffer);

        // Parse the command and arguments
        char *cmd_name;
        char *args;
        parse_command(cli_buffer, &cmd_name, &args);

        // Execute the command
        int found = 0;
        for (int i = 0; i < num_commands; i++)
        {
            if (strcmp(cmd_name, commands[i].name) == 0)
            {
                commands[i].execute(args);
                found = 1;
                break;
            }
        }
        if (!found && cli_buffer[0] != '\0')
        {
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

// === Command Implementations ===

/**
 * @brief Command: Show help information for all commands or a specific command.
 *
 * @param args If NULL, lists all commands with brief descriptions. If a command name,
 *             shows detailed help for that command.
 */
void cmd_help(char *args)
{
    if (args == NULL)
    {
        // Show brief information for all commands
        uart_puts("\nAvailable commands:\n");
        for (int i = 0; i < num_commands; i++)
        {
            uart_puts(commands[i].name);
            uart_puts(" - ");
            uart_puts(commands[i].brief);
            uart_puts("\n");
        }
    }
    else
    {
        // Show detailed information for a specific command
        int found = 0;
        for (int i = 0; i < num_commands; i++)
        {
            if (strcmp(args, commands[i].name) == 0)
            {
                uart_puts("\n");
                uart_puts(commands[i].details);
                uart_puts("\n");
                found = 1;
                break;
            }
        }
        if (!found)
        {
            uart_puts("\nCommand not found: ");
            uart_puts(args);
            uart_puts("\n");
        }
    }
}

/**
 * @brief Command: Clear the terminal screen by scrolling.
 *
 * @param args Not used.
 */
void cmd_clear(char *args)
{
    // Scroll down to "clear" the screen by printing newlines
    for (int i = 0; i < 30; i++)
    {
        uart_puts("\n");
    }
}

/**
 * @brief Command: Show board revision and MAC address.
 *
 * @param args Not used.
 * @todo Implement this command to fetch and display board information.
 */
void cmd_showinfo(char *args)
{
    uint32_t revision = get_board_revision();
    if (revision == 0)
    {
        uart_puts("\nFailed to get board revision.\n");
    }
    else
    {
        uart_puts("\nBoard Revision: ");
        uart_hex(revision);
        uart_puts("\n");
        print_board_info(revision); // Print board information based on revision  
        uart_puts("\n");
    }

    unsigned char mac[6];
    if (get_mac_address(mac))
    {
        uart_puts("\nMAC Address: ");
        for (int i = 0; i < 6; i++)
        {
            uart_hex(mac[i]);
            if (i < 5)
                uart_puts(":");
        }
        uart_puts("\n");
    }
    else
    {
        uart_puts("\nFailed to get MAC address.\n");
    }
}

/**
 * @brief Command: Change the baudrate of the current UART.
 *
 * @param args The new baudrate value (e.g., "9600").
 */
void cmd_baudrate(char *args)
{
    // TODO: Parse args to get baudrate, validate, and update UART settings.
    // Supported baudrates: 9600, 19200, 38400, 57600, 115200.

    if (args == NULL)
    {
        uart_puts("Error: No baudrate specified.\n");
        uart_puts("Usage: baudrate <9600|19200|38400|57600|115200>\n");
        return;
    }

    int baudrate = str_to_int(args); // Convert argument to integer 

    // Validate the baudrate
    switch (baudrate)
    {
        case 9600:
        case 19200:
        case 38400:
        case 57600:
        case 115200:
            uart_puts("Setting baudrate to ");
            uart_dec(baudrate);
            uart_puts("...\n");

            if (set_uart_baudrate(baudrate) == 0)
            {
                uart_puts("Baudrate successfully updated.\n");
            }
            else
            {
                uart_puts("Failed to update baudrate.\n");
            }
            break;

        default:
            uart_puts("Error: Unsupported baudrate.\n");
            uart_puts("Usage: baudrate <9600|19200|38400|57600|115200>\n");
            break;
    }
}


/**
 * @brief Command: Turn on/off CTS/RTS handshaking on the current UART.
 *
 * @param args "on" to enable, "off" to disable handshaking.
 * @todo Implement this command to toggle UART handshaking.
 */
void cmd_handshake(char *args)
{
    uart_puts("\nhandshake: Not yet implemented.\n");
    // TODO: Parse args ("on" or "off") and configure UART handshaking.
}

// === Welcome Message ===

/**
 * @brief Display the BitOS welcome banner with animation.
 *
 * Prints the ASCII art banner line by line with a delay, followed by a blinking
 * "BitOS Loaded!" message.
 */
void os_welcome()
{
    // Array of banner lines
    const char *banner[] = {
        "     888                                      .d88888b.   .d8888b.  ",
        "     888                                     d88P\" \"Y88b d88P  Y88b ",
        "     888                                     888     888 Y88b.      ",
        " .d88888 888  888  .d88b.  88888b.   .d88b.  888     888  \"Y888b.   ",
        "d88\" 888 888  888 d88\"\"88b 888 \"88b d88P\"88b 888     888     \"Y88b. ",
        "888  888 888  888 888  888 888  888 888  888 888     888       \"888 ",
        "Y88b 888 Y88b 888 Y88..88P 888  888 Y88b 888 Y88b. .d88P Y88b  d88P ",
        " \"Y88888  \"Y88888  \"Y88P\"  888  888  \"Y88888  \"Y88888P\"   \"Y8888P\"  ",
        "                                         888                        ",
        "                                    Y8b d88P                        ",
        "                                     \"Y88P\"                         ",
        NULL // End marker
    };

    // Print banner line by line with delay
    uart_puts("\n\n");
    for (int i = 0; banner[i] != NULL; i++)
    {
        uart_puts(banner[i]);
        uart_puts("\n");
        delay(200); // 200ms delay between lines
    }

    uart_puts("Developed By Thai Duong, only Thai Duong - S39878955.\n\n");
}

/**
 * @brief Main entry point for BitOS.
 *
 * Initializes the UART, displays the welcome message, and enters the CLI loop.
 */


 /**
 * @brief Find matching commands for autocompletion.
 * 
 * @param partial The partial command to match against.
 * @param matches Array to store pointers to matching commands.
 * @return int Number of matches found.
 */
int find_matches(const char *partial, Command **matches)
{
    int count = 0;
    int len = strlen(partial);
    
    for (int i = 0; i < num_commands; i++)
    {
        if (strncmp(partial, commands[i].name, len) == 0)
        {
            matches[count++] = &commands[i];
        }
    }
    
    return count;
}

/**
 * @brief Find the common prefix of all matching commands.
 * 
 * @param matches Array of matching commands.
 * @param count Number of matches.
 * @param buffer Buffer to store the common prefix.
 */
void find_common_prefix(Command **matches, int count, char *buffer)
{
    if (count == 0) return;
    
    // Start with the first match
    strcpy(buffer, matches[0]->name);
    int prefix_len = strlen(buffer);
    
    // Compare with other matches
    for (int i = 1; i < count; i++)
    {
        int j = 0;
        while (j < prefix_len && matches[i]->name[j] == buffer[j])
        {
            j++;
        }
        prefix_len = j; // Adjust prefix length
        buffer[prefix_len] = '\0';
    }
}

/**
 * @brief Handle TAB key for command autocompletion.
 * 
 * @param cli_buffer Current command buffer.
 * @param index Pointer to the current position in the buffer.
 * @return int 1 if display should be updated, 0 otherwise.
 */
int handle_tab_completion(char *cli_buffer, int *index)
{
    // Split the buffer into command and args
    char buffer_copy[MAX_CMD_SIZE];
    strncpy(buffer_copy, cli_buffer, MAX_CMD_SIZE);
    
    char *cmd_part = buffer_copy;
    char *args_part = NULL;
    
    // Find the first space to separate command from args
    char *space = strchr(buffer_copy, ' ');
    if (space)
    {
        *space = '\0';
        args_part = space + 1;
        // Skip any additional spaces
        while (*args_part == ' ')
            args_part++;
    }
    
    // Check if we're completing a command or an argument
    if (args_part == NULL || args_part[0] == '\0')
    {
        // Command completion
        Command *matches[num_commands];
        int match_count = find_matches(cmd_part, matches);
        
        if (match_count == 0)
        {
            // No matches
            return 0;
        }
        else if (match_count == 1)
        {
            // Single match - complete fully
            strcpy(cli_buffer, matches[0]->name);
            *index = strlen(cli_buffer);
            cli_buffer[(*index)++] = ' ';
            cli_buffer[*index] = '\0';
            return 1;
        }
        else
        {
            // Multiple matches
            char common[MAX_CMD_SIZE];
            find_common_prefix(matches, match_count, common);
            
            if (strlen(common) > strlen(cmd_part))
            {
                // We can complete partially
                strcpy(cli_buffer, common);
                *index = strlen(cli_buffer);
                return 1;
            }
            else
            {
                // Show all options
                uart_puts("\n");
                for (int i = 0; i < match_count; i++)
                {
                    uart_puts(matches[i]->name);
                    uart_puts("  ");
                }
                uart_puts("\n");
                return 1;
            }
        }
    }
    else
    {
        // Argument completion
        // First, identify the command
        for (int i = 0; i < num_commands; i++)
        {
            if (strcmp(cmd_part, commands[i].name) == 0)
            {
                // Special handling for specific commands that have known arguments
                if (strcmp(commands[i].name, "baudrate") == 0)
                {
                    // Baudrate options: 9600, 19200, 38400, 57600, 115200
                    const char *options[] = {"9600", "19200", "38400", "57600", "115200"};
                    const int num_options = 5;
                    int matches_found = 0;
                    const char *matching_options[num_options];
                    
                    for (int j = 0; j < num_options; j++)
                    {
                        if (strncmp(args_part, options[j], strlen(args_part)) == 0)
                        {
                            matching_options[matches_found++] = options[j];
                        }
                    }
                    
                    if (matches_found == 1)
                    {
                        // Single match - complete fully
                        sprintf(cli_buffer, "%s %s", cmd_part, matching_options[0]);
                        *index = strlen(cli_buffer);
                        return 1;
                    }
                    else if (matches_found > 1)
                    {
                        // Show options
                        uart_puts("\n");
                        for (int j = 0; j < matches_found; j++)
                        {
                            uart_puts(matching_options[j]);
                            uart_puts("  ");
                        }
                        uart_puts("\n");
                        return 1;
                    }
                }
                else if (strcmp(commands[i].name, "handshake") == 0)
                {
                    // Handshake options: on, off
                    const char *options[] = {"on", "off"};
                    const int num_options = 2;
                    int matches_found = 0;
                    const char *matching_options[num_options];
                    
                    for (int j = 0; j < num_options; j++)
                    {
                        if (strncmp(args_part, options[j], strlen(args_part)) == 0)
                        {
                            matching_options[matches_found++] = options[j];
                        }
                    }
                    
                    if (matches_found == 1)
                    {
                        // Single match - complete fully
                        sprintf(cli_buffer, "%s %s", cmd_part, matching_options[0]);
                        *index = strlen(cli_buffer);
                        return 1;
                    }
                    else if (matches_found > 1)
                    {
                        // Show options
                        uart_puts("\n");
                        for (int j = 0; j < matches_found; j++)
                        {
                            uart_puts(matching_options[j]);
                            uart_puts("  ");
                        }
                        uart_puts("\n");
                        return 1;
                    }
                }
                else if (strcmp(commands[i].name, "help") == 0)
                {
                    // Help can autocomplete with command names
                    Command *matches[num_commands];
                    int match_count = 0;
                    
                    for (int j = 0; j < num_commands; j++)
                    {
                        if (strncmp(args_part, commands[j].name, strlen(args_part)) == 0)
                        {
                            matches[match_count++] = &commands[j];
                        }
                    }
                    
                    if (match_count == 1)
                    {
                        // Single match - complete fully
                        sprintf(cli_buffer, "%s %s", cmd_part, matches[0]->name);
                        *index = strlen(cli_buffer);
                        return 1;
                    }
                    else if (match_count > 1)
                    {
                        // Show options
                        uart_puts("\n");
                        for (int j = 0; j < match_count; j++)
                        {
                            uart_puts(matches[j]->name);
                            uart_puts("  ");
                        }
                        uart_puts("\n");
                        return 1;
                    }
                }
                break;
            }
        }
    }
    
    return 0;
}