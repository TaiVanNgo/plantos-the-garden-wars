#ifndef CLI_H
#define CLI_H

// Include necessary headers
#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"


// Define NULL for our use
#ifndef NULL
#define NULL ((void *)0)
#endif

// Define constants
#define MAX_CMD_SIZE 100
#define PROMPT "NexOS> "
// Command history setup
#define MAX_HISTORY 10
static char history[MAX_HISTORY][MAX_CMD_SIZE];
static int history_count = 0;
static int history_pos = 0; // Current position in history for navigation
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
void add_to_history(char* command);
#endif // CLI_H