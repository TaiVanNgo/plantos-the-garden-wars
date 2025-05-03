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

/**
 * @brief Compare at most n characters of two strings.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @param n The maximum number of characters to compare.
 * @return int 0 if equal, non-zero otherwise.
 */
int strncmp(const char *s1, const char *s2, unsigned int n)
{
    while (n && *s1 && (*s1 == *s2))
    {
        ++s1;
        ++s2;
        --n;
    }
    if (n == 0)
    {
        return 0;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

/**
 * @brief Calculate the length of a string.
 *
 * @param s The string.
 * @return unsigned int The number of characters in the string.
 */
unsigned int strlen(const char *s)
{
    const char *p = s;
    while (*p)
        p++;
    return p - s;
}

/**
 * @brief Copy a string to a destination buffer.
 *
 * @param dest The destination buffer.
 * @param src The source string.
 * @return char* The destination buffer.
 */
char *strcpy(char *dest, const char *src)
{
    char *d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

/**
 * @brief Copy at most n characters from a string to a destination buffer.
 *
 * @param dest The destination buffer.
 * @param src The source string.
 * @param n The maximum number of characters to copy.
 * @return char* The destination buffer.
 */
char *strncpy(char *dest, const char *src, unsigned int n)
{
    unsigned int i;

    for (i = 0; i < n && src[i] != '\0'; i++)
        dest[i] = src[i];
    for (; i < n; i++)
        dest[i] = '\0';

    return dest;
}

/**
 * @brief Find the first occurrence of a character in a string.
 *
 * @param s The string to search.
 * @param c The character to find.
 * @return char* Pointer to the first occurrence, or NULL if not found.
 */
char *strchr(const char *s, int c)
{
    while (*s != (char)c)
    {
        if (!*s++)
            return NULL;
    }
    return (char *)s;
}

/**
 * @brief Format a string according to the format specifier and arguments.
 * This is a simplified version of sprintf that only supports %s and %d.
 *
 * @param str The destination buffer.
 * @param format The format string.
 * @param ... The arguments to format.
 * @return int The number of characters written.
 */
int sprintf(char *str, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    
    char *s_val;
    int d_val;
    char *ptr = str;
    
    for (const char *fp = format; *fp; fp++)
    {
        if (*fp != '%')
        {
            *ptr++ = *fp;
            continue;
        }
        
        fp++; // Skip the %
        
        switch (*fp)
        {
            case 's':
                s_val = va_arg(args, char *);
                while (*s_val)
                {
                    *ptr++ = *s_val++;
                }
                break;
                
            case 'd':
                d_val = va_arg(args, int);
                
                // Handle negative numbers
                if (d_val < 0)
                {
                    *ptr++ = '-';
                    d_val = -d_val;
                }
                
                // Convert integer to string
                if (d_val == 0)
                {
                    *ptr++ = '0';
                }
                else
                {
                    // Find number of digits
                    int temp = d_val;
                    int num_digits = 0;
                    while (temp > 0)
                    {
                        temp /= 10;
                        num_digits++;
                    }
                    
                    // Convert each digit
                    char digits[20]; // Max 20 digits for int
                    for (int i = num_digits - 1; i >= 0; i--)
                    {
                        digits[i] = '0' + (d_val % 10);
                        d_val /= 10;
                    }
                    
                    // Copy digits to output
                    for (int i = 0; i < num_digits; i++)
                    {
                        *ptr++ = digits[i];
                    }
                }
                break;
                
            default:
                *ptr++ = *fp;
                break;
        }
    }
    
    *ptr = '\0'; // Null-terminate the string
    va_end(args);
    
    return ptr - str;
}
