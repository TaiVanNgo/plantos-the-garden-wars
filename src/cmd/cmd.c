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

 #include "../../include/cmd.h"
 #include "../../include/utils.h"
 #include "cmd-utils.c"
 
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
      "Example: BitOS> handshake on\n"
      "Example: BitOS> handshake off",
      cmd_handshake}
 };
 
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
        char next = getUart(); // Should be '['
        if (next == '[')
        {
            char arrow = getUart(); // Should be 'A' (up) or 'B' (down)
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
    if (args == NULL)
    {
        uart_puts("\nUsage: handshake <on|off>\n");
        uart_puts("Example: BitOS> handshake on\n");
        return;
    }

    if (strcmp(args, "on") == 0)
    {
        uart_puts("\nCTS/RTS handshaking enabled.\n");
        uart_toggle_rts_cts();  // Toggle to enable RTS/CTS
    }
    else if (strcmp(args, "off") == 0)
    {
        // Disable CTS/RTS handshaking
        uart_puts("\nCTS/RTS handshaking disabled.\n");
        uart_toggle_rts_cts();  // Toggle to disable RTS/CTS
    }
    else
    {
        uart_puts("\nInvalid argument: ");
        uart_puts(args);
        uart_puts("\nUsage: handshake <on|off>\n");
        uart_puts("Example: BitOS> handshake on\n");
    }
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
         delay(150); // 200ms delay between lines
     }
 
     uart_puts("Developed By Thai Duong, only Thai Duong - S39878955.\n\n");
     uart_puts("BitOS Loaded!\n\n");
 }
 
 /**
  * @brief Main entry point for BitOS.
  *
  * Initializes the UART, displays the welcome message, and enters the CLI loop.
  */

