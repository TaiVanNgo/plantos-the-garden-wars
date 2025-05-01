#include "utils.h"

/**
 * @brief Compare two strings.
 *
 * @param s1 First string to compare.
 * @param s2 Second string to compare.
 * @return Negative if s1 < s2, zero if s1 == s2, positive if s1 > s2.
 */
int strcmp(const char *s1, const char *s2)
{
  while (*s1 && (*s1 == *s2))
  {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void delay_ms(unsigned int ms)
{
  // Each iteration takes about 4 cycles
  // ARM CPU frequency is typically 1GHz = 1,000,000,000 cycles per second
  // So for 1ms we need 250,000 iterations (1,000,000,000 / (4 * 1000))
  unsigned int count = ms * 250000;
  for (unsigned int i = 0; i < count; i++)
  {
    asm volatile("nop"); // No operation, prevents optimization
  }
}

/**
 * @brief Simple string to integer conversion function
 *
 * @param str The string to convert
 * @return int The converted integer value, or -1 if invalid
 */
int str_to_int(const char *str)
{
    int result = 0;
    
    // Handle null pointer
    if (str == NULL)
        return -1;
    
    // Skip leading whitespace
    while (*str == ' ' || *str == '\t')
        str++;
    
    // Check if string is empty
    if (*str == '\0')
        return -1;
    
    // Process each digit
    while (*str >= '0' && *str <= '9') {
        // Check for overflow
        if (result > (__INT_MAX__ - (*str - '0')) / 10)
            return -1;
        
        result = result * 10 + (*str - '0');
        str++;
    }
    
    // Check if there are non-digit characters
    if (*str != '\0' && *str != ' ' && *str != '\t' && *str != '\r' && *str != '\n')
        return -1;
    
    return result;
}
