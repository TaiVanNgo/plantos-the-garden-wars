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
