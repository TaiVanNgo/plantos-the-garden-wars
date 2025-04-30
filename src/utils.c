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
