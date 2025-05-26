#include "cmd.h"
#include "utils.h"

// Autocompletion Functions
int handle_tab_completion(char *cli_buffer, int *index);
int find_matches(const char *partial, Command **matches);
void find_common_prefix(Command **matches, int count, char *buffer);