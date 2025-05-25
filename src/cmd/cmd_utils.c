#include "../../include/cmd.h"
#include "../../include/utils.h"
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
    if (args_part == NULL)
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
                if (strcmp(commands[i].name, "baud") == 0)
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