#define MAX_CMD_SIZE 100
#ifndef CLI_H
#define CLI_H

// Include necessary headers
#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "image.h"

// Define constants
#define MAX_CMD_SIZE 100
#define PROMPT "GROS> "

// Command structure to hold command details
typedef struct {
    const char* name;         // Command name
    const char* brief;        // Brief description for help command
    const char* details;      // Detailed description for help <command_name>
    void (*execute)(char* args); // Function to execute the command
} Command;

// Declare the command table and its size as extern (defined in cli.c)
extern Command commands[];
extern const int num_commands;

// Function prototypes
void cli(void);
void clear_line(void);
void display_prompt(char* buffer);
void parse_command(char* buffer, char** cmd_name, char** args);

// Command function prototypes
void cmd_help(char* args);
void cmd_clear(char* args);
void cmd_showinfo(char* args);
void cmd_baudrate(char* args);
void cmd_handshake(char* args);

#endif // CLI_H
void os_welcome();