/**
 * @file cli.c
 * @brief Command Line Interface (CLI) implementation for PlantOS.
 *
 * This file implements the CLI for PlantOS, a lightweight operating system designed
 * for embedded systems (e.g., Raspberry Pi). The CLI allows users to interact with
 * the system via UART by entering commands such as "help", "clear", etc.
 * Features include command parsing, history navigation (up/down arrow keys),
 * and a welcome banner.
 *
 * @author Thai Duong (S39878955)
 */

// --- Includes ---
#include "../../include/cmd.h"
#include "../../include/utils.h"
#include "../../include/video.h"
#include "cmd_utils.c"

// --- Command Table and Declarations ---
void cmd_video(char *args);

Command commands[] = {
    {"help", "Show help", "help [cmd] - Show help info", cmd_help},
    {"clear", "Clear screen", "clear - Clear the terminal", cmd_clear},
    {"showinfo", "Board info", "showinfo - Show board revision and MAC", cmd_showinfo},
    {"baudrate", "Set baudrate", "baudrate <rate> - Set UART baudrate", cmd_baudrate},
    {"handshake", "RTS/CTS", "handshake <on|off> - Toggle handshaking", cmd_handshake},
    {"game", "Start game", "game - Start Garden Wars", cmd_game},
    {"video", "Play video", "video - Play the intro video animation", cmd_video}};
const int num_commands = sizeof(commands) / sizeof(commands[0]);

// --- Command History Management ---
static char history[MAX_HISTORY][MAX_CMD_SIZE];
static int history_count = 0;
static int history_pos = 0;

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

// --- CLI Core Functions ---

/**
 * @brief Clear the current line on the terminal.
 *
 * Overwrites the line with spaces and returns the cursor to the start.
 */
void clear_line()
{
    uart_puts("\r");   // Return to start of line
    uart_puts(PROMPT); // Print prompt
}

/**
 * @brief Display the CLI prompt and current buffer contents.
 *
 * @param buffer The current command buffer to display.
 */
void display_prompt(char *buffer)
{
    uart_puts(buffer); // Just print the buffer contents
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
    static int escape_state = 0;          // State for parsing escape sequences
    static char escape_seq[3];            // Buffer for escape sequence

    // Check if there's a character available and read it
    char c = getUart();
    if (c == 0)
    {
        return; // Exit if no character was available
    }

    // Handle escape sequence state machine
    if (escape_state > 0)
    {
        escape_seq[escape_state] = c;
        escape_state++;

        if (escape_state == 3)
        {
            escape_state = 0; // Reset state

            if (escape_seq[1] == '[')
            {
                if (escape_seq[2] == 'A')
                { // Up arrow
                    if (history_count > 0 && history_pos > 0)
                    {
                        history_pos--;
                        strncpy(cli_buffer, history[history_pos], MAX_CMD_SIZE - 1);
                        cli_buffer[MAX_CMD_SIZE - 1] = '\0';
                        index = strlen(cli_buffer);
                        clear_line();
                        display_prompt(cli_buffer);
                    }
                    return;
                }
                else if (escape_seq[2] == 'B')
                { // Down arrow
                    if (history_pos < history_count)
                    {
                        history_pos++;
                        if (history_pos == history_count)
                        {
                            cli_buffer[0] = '\0';
                            index = 0;
                        }
                        else
                        {
                            strncpy(cli_buffer, history[history_pos], MAX_CMD_SIZE - 1);
                            cli_buffer[MAX_CMD_SIZE - 1] = '\0';
                            index = strlen(cli_buffer);
                        }
                        clear_line();
                        display_prompt(cli_buffer);
                    }
                    return;
                }
            }
        }
        return;
    }

    // Start of escape sequence
    if (c == 27)
    {
        escape_state = 1;
        escape_seq[0] = c;
        return;
    }

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

    // Handle backspace (ASCII 127) or delete (ASCII 8)
    if (c == 127 || c == 8)
    {
        if (index > 0)
        {
            index--;
            cli_buffer[index] = '\0';
            // Send backspace, space, and backspace to clear the character
            uart_sendc('\b');
            uart_sendc(' ');
            uart_sendc('\b');
        }
        return;
    }

    // Handle new character input (normal characters)
    if (c != '\n' && c != '\r')
    {
        if (index < MAX_CMD_SIZE - 1)
        {
            cli_buffer[index] = c;
            index++;
            cli_buffer[index] = '\0';
            uart_sendc(c); // Echo the character directly
        }
        return;
    }

    // When newline is received, process the command
    if (c == '\n' || c == '\r')
    {
        uart_puts("\n");

        // Only add non-empty commands to history
        if (cli_buffer[0] != '\0')
        {
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

            if (!found)
            {
                uart_puts("Unknown command. Type 'help' to see available commands.\n");
            }
        }

        // Reset the buffer for the next command
        index = 0;
        cli_buffer[0] = '\0';
        history_pos = history_count;
        uart_puts(PROMPT);
        return;
    }
}

// --- Command Implementations ---

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
            uart_mac_hex(mac[i]);
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
    if (baudrate == 9600 || baudrate == 19200 || baudrate == 38400 || baudrate == 57600 || baudrate == 115200)
    {
        uart_puts("Setting baudrate to ");
        uart_dec(baudrate);
        uart_puts("...\n");

        if (uart_init_with_baudrate(baudrate) == 0)
        {
            uart_puts("Baudrate successfully updated.\n");
        }
        else
        {
            uart_puts("Failed to update baudrate.\n");
        }
    }
    else
    {
        uart_puts("Error: Unsupported baudrate.\n");
        uart_puts("Usage: baudrate <9600|19200|38400|57600|115200>\n");
    }
}

/**
 * @brief Command: Turn on/off CTS/RTS handshaking on the current UART.
 *
 * @param args "on" to enable, "off" to disable handshaking.
 */
void cmd_handshake(char *args)
{
    if (args == NULL)
    {
        uart_puts("\nUsage: handshake <on|off>\n");
        uart_puts("Example: PlantOS> handshake on\n");
        return;
    }

    if (strcmp(args, "on") == 0)
    {
        uart_puts("\nCTS/RTS handshaking enabled.\n");
        RTS_CTS_init(); // Toggle to enable RTS/CTS
    }
    else if (strcmp(args, "off") == 0)
    {
        // Disable CTS/RTS handshaking
        uart_puts("\nCTS/RTS handshaking disabled.\n");
        uart_init(); // Toggle to disable RTS/CTS
    }
    else
    {
        uart_puts("\nInvalid argument: ");
        uart_puts(args);
        uart_puts("\nUsage: handshake <on|off>\n");
        uart_puts("Example: PlantOS> handshake on\n");
    }
}

/**
 * @brief Command: Start the Plants vs. Zombies game.
 *
 */
void cmd_game()
{
    uart_puts("\nStarting Garden Wars ...\n");
    game_main();
}

/**
 * @brief Command: Play the intro video animation.
 *
 * @param args Not used.
 */
void cmd_video(char *args)
{
    uart_puts("\nPlaying intro video...\n");
    clear_screen();
    
    // Initialize video
    Video vid;
    video_init(&vid);
    
    // Play video at position (80, 120) with all frames
    play_video(&vid, 80, 120, vid.total_frames);
    
    uart_puts("Video playback completed!\n");
}

// --- Welcome Message ---

/**
 * @brief Display the PlantOS welcome banner immediately.
 *
 */
void os_welcome()
{
    // Array of banner lines
    const char *banner[] = {
        "8888888b.  888                   888     .d88888b.   .d8888b.  ",
        "888   Y88b 888                   888    d88P\" \"Y88b d88P  Y88b ",
        "888    888 888                   888    888     888 Y88b.      ",
        "888   d88P 888  8888b.  88888b.  888888 888     888  \"Y888b.   ",
        "8888888P\"  888     \"88b 888 \"88b 888    888     888     \"Y88b. ",
        "888        888 .d888888 888  888 888    888     888       \"888 ",
        "888        888 888  888 888  888 Y88b.  Y88b. .d88P Y88b  d88P ",
        "888        888 \"Y888888 888  888  \"Y888  \"Y88888P\"   \"Y8888P\"  ",
        NULL};

    uart_puts("\n\n");
    for (int i = 0; banner[i] != NULL; i++)
    {
        uart_puts(banner[i]);
        uart_puts("\n");
    }

    uart_puts("PlantOS Loaded!\n\n");
}
