/**
 * @file cli.h
 * @brief Header file for the Command Line Interface (CLI) of NexOS.
 *
 * This file defines the constants, data structures, and function prototypes
 * for the CLI module of NexOS, a lightweight operating system for embedded systems.
 * The CLI allows users to interact with the system via UART by entering commands.
 */

#ifndef CLI_H
#define CLI_H

// Include necessary headers for UART and hardware-specific functionality
#include "uart0.h"
#include "uart1.h"
#include "mbox.h"
#include "framebf.h"

// Define NULL for our use (since we're not using the standard library)
#ifndef NULL
#define NULL ((void *)0)
#endif

// CLI configuration constants
#define MAX_CMD_SIZE 100 // Maximum length of a command string
#define MAX_HISTORY 10   // Maximum number of commands to store in history
#define PROMPT "NexOS> " // CLI prompt displayed to the user

/**
 * @brief Structure to define a CLI command.
 *
 * Each command has a name, a brief description, detailed help text, and
 * a function pointer to its implementation.
 */
typedef struct
{
    const char *name;            // Command name (e.g., "help")
    const char *brief;           // Brief description for help command
    const char *details;         // Detailed description for help <command_name>
    void (*execute)(char *args); // Function to execute the command
} Command;

// Declare the command table and its size as extern (defined in cli.c)
extern Command commands[];
extern const int num_commands;

// Function prototypes for CLI operations
void cli(void);                                                 // Main CLI loop
void clear_line(void);                                          // Clear the current terminal line
void display_prompt(char *buffer);                              // Display the CLI prompt and buffer
void parse_command(char *buffer, char **cmd_name, char **args); // Parse command and args
void add_to_history(char *command);                             // Add a command to history

// Command function prototypes
void cmd_help(char *args);      // Show help for commands
void cmd_clear(char *args);     // Clear the terminal screen
void cmd_showinfo(char *args);  // Show board information (not implemented)
void cmd_baudrate(char *args);  // Change UART baudrate (not implemented)
void cmd_handshake(char *args); // Toggle UART handshaking (not implemented)
void os_welcome(void);

#endif // CLI_H