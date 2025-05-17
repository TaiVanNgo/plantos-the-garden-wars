// ----------------------------------- framebf.c
// -------------------------------------
#include "../include/framebf.h"
#include "../../assets/fonts/fonts.h"
#include "../include/mbox.h"
#include "../include/uart0.h"
#include "../include/uart1.h"
#include "../include/video.h"

// Use RGBA32 (32 bits for each pixel)
#define COLOR_DEPTH 32

// Pixel Order: BGR in memory order (little endian --> RGB in byte order)
#define PIXEL_ORDER 0

// Screen info
unsigned int width, height, pitch;

/* Frame buffer address
 * (declare as pointer of unsigned char to access each byte) */
unsigned char *fb;

/**
 * Set screen resolution to 800x600
 */
void framebf_init()
{
  mBuf[0] = 35 * 4; // Length of message in bytes
  mBuf[1] = MBOX_REQUEST;

  mBuf[2] = MBOX_TAG_SETPHYWH; // Set physical width-height
  mBuf[3] = 8;                 // Value size in bytes
  mBuf[4] = 0;                 // REQUEST CODE = 0
  mBuf[5] = PHYSICAL_WIDTH;    // Value(width)
  mBuf[6] = PHYSICAL_HEIGHT;   // Value(height)

  mBuf[7] = MBOX_TAG_SETVIRTWH; // Set virtual width-height
  mBuf[8] = 8;
  mBuf[9] = 0;
  mBuf[10] = VIRTUAL_WIDTH;
  mBuf[11] = VIRTUAL_HEIGHT;

  mBuf[12] = MBOX_TAG_SETVIRTOFF; // Set virtual offset
  mBuf[13] = 8;
  mBuf[14] = 0;
  mBuf[15] = 0; // x offset
  mBuf[16] = 0; // y offset

  mBuf[17] = MBOX_TAG_SETDEPTH; // Set color depth
  mBuf[18] = 4;
  mBuf[19] = 0;
  mBuf[20] = COLOR_DEPTH; // Bits per pixel

  mBuf[21] = MBOX_TAG_SETPXLORDR; // Set pixel order
  mBuf[22] = 4;
  mBuf[23] = 0;
  mBuf[24] = PIXEL_ORDER;

  mBuf[25] = MBOX_TAG_GETFB; // Get frame buffer
  mBuf[26] = 8;
  mBuf[27] = 0;
  mBuf[28] = 16; // alignment in 16 bytes
  mBuf[29] = 0;  // will return Frame Buffer size in bytes

  mBuf[30] = MBOX_TAG_GETPITCH; // Get pitch
  mBuf[31] = 4;
  mBuf[32] = 0;
  mBuf[33] = 0; // Will get pitch value here

  mBuf[34] = MBOX_TAG_LAST;

  // Call Mailbox
  if (mbox_call(ADDR(mBuf), MBOX_CH_PROP) // mailbox call is successful ?
      && mBuf[20] == COLOR_DEPTH          // got correct color depth ?
      && mBuf[24] == PIXEL_ORDER          // got correct pixel order ?
      && mBuf[28] != 0                    // got a valid address for frame buffer ?
  )
  {

    /* Convert GPU address to ARM address (clear higher address bits)
     * Frame Buffer is located in RAM memory, which VideoCore MMU
     * maps it to bus address space starting at 0xC0000000.
     * Software accessing RAM directly use physical addresses
     * (based at 0x00000000)
     */
    mBuf[28] &= 0x3FFFFFFF;

    // Access frame buffer as 1 byte per each address
    fb = (unsigned char *)((unsigned long)mBuf[28]);
    uart_puts("Got allocated Frame Buffer at RAM physical address: ");
    uart_hex(mBuf[28]);
    uart_puts("\n");

    uart_puts("Frame Buffer Size (bytes): ");
    uart_dec(mBuf[29]);
    uart_puts("\n");

    width = mBuf[5];  // Actual physical width
    height = mBuf[6]; // Actual physical height
    pitch = mBuf[33]; // Number of bytes per line
  }
  else
  {
    uart_puts("Unable to get a frame buffer with provided setting\n");
  }
}

void draw_pixel(int x, int y, unsigned int attr)
{
  int offs = (y * pitch) + (COLOR_DEPTH / 8 * x);

  /*	//Access and assign each byte
   *(fb + offs    ) = (attr >> 0 ) & 0xFF; //BLUE  (get the least significant
   *byte) (fb + offs + 1) = (attr >> 8 ) & 0xFF; //GREEN (fb + offs + 2) = (attr
   *>> 16) & 0xFF; //RED (fb + offs + 3) = (attr >> 24) & 0xFF; //ALPHA
   */

  // Access 32-bit together
  *((unsigned int *)(fb + offs)) = attr;
}

void draw_rect(int x1, int y1, int x2, int y2, unsigned int attr, int fill)
{
  for (int y = y1; y <= y2; y++)
    for (int x = x1; x <= x2; x++)
    {
      if ((x == x1 || x == x2) || (y == y1 || y == y2))
        draw_pixel(x, y, attr);
      else if (fill)
        draw_pixel(x, y, attr);
    }
}

// Function to draw line
void draw_line(int x1, int y1, int x2, int y2, unsigned int attr)
{
  for (int x = x1; x <= x2; x++)
  {
    int y = (float)(y1 - y2) / (x1 - x2) * (x - x1) + y1;
    draw_pixel(x, y, attr);
  }
}

// Function to calculate the square root of a number using the Newton-Raphson
// method
double sqrt(double number)
{
  if (number < 0)
  {
    return -1; // Return -1 for negative inputs as square root of negative is
               // not defined in real numbers
  }

  double tolerance = 0.000001; // Define the tolerance for the result
  double guess = number / 2.0; // Initial guess (can be any positive number,
                               // here half of the number)
  double result = 0.0;

  while (1)
  {
    result = 0.5 * (guess + number / guess); // Calculate the next approximation

    // Check if the difference between the current guess and the new result is
    // within the tolerance
    int diff = (result > guess) ? (result - guess) : (guess - result);
    if (diff < tolerance)
    {
      break;
    }

    guess = result; // Update the guess for the next iteration
  }

  return result;
}

// Function to draw circle
void draw_circle(int center_x, int center_y, int radius, unsigned int attr,
                 int fill)
{
  // Draw the circle when going on x side
  for (int x = center_x - radius; x <= center_x + radius; x++)
  {
    // Calculate the corresponding y values using the circle equation
    int dy = sqrt(radius * radius - (x - center_x) * (x - center_x));
    int upper_y = center_y + dy;
    int lower_y = center_y - dy;

    draw_pixel(x, upper_y, attr);
    draw_pixel(x, lower_y, attr);

    // Fill the circle, draw a line between lower_y and upper_y
    if (fill)
    {
      for (int y = lower_y; y <= upper_y; y++)
      {
        draw_pixel(x, y, attr);
      }
    }
  }

  /* Also draw the circle border when going on y side (
  since some points may be missing due to inaccurate calculation above) */

  for (int y = center_y - radius; y <= center_y + radius; y++)
  {
    // Calculate the corresponding x values using the circle equation
    int dx = sqrt(radius * radius - (y - center_y) * (y - center_y));

    int left_x = center_x - dx;
    int right_x = center_x + dx;

    draw_pixel(left_x, y, attr);
    draw_pixel(right_x, y, attr);
  }
}

void draw_image(const unsigned int pixel_data[], int pos_x, int pos_y,
                int width, int height)
{
  for (int i = 0; i < width * height; i++)
  {
    int x = pos_x + (i % width);
    int y = pos_y + (i / width);
    draw_pixel(x, y, pixel_data[i]);
  }
}

void clear_screen()
{
  // Total number of 32-bit pixels
  unsigned int pixel_count = height * pitch / 4;

  // Get pointer to framebuffer as 32-bit integers
  unsigned int *fb_ptr = (unsigned int *)fb;

  // Fill entire framebuffer with zeros (black)
  for (unsigned int i = 0; i < pixel_count; i++)
  {
    fb_ptr[i] = BLACK;
  }
}

/**
 * Draws a single character at the specified position
 *
 * @param ch character to show
 * @param x x-coordinate for the top-left corner
 * @param y y-coordinate for the top-left corner
 * @param attr the color for character (32-bit ARGB format)
 * @param scale
 */
void draw_char(unsigned char ch, int x, int y, unsigned int attr, int scale)
{
  // Calculate pointer to the glyph data for the given character
  // If the character out of range, default to the first glyph
  unsigned char *glyph =
      (unsigned char *)&font + (ch < FONT_NUMGLYPHS ? ch : 0) * FONT_BPG;

  // Loop through each row of the character
  for (int i = 0; i < FONT_HEIGHT * scale; i++)
  {
    // Loop through each column of the character
    for (int j = 0; j < FONT_WIDTH * scale; j++)
    {
      // Extract current pixel
      // unsigned char mask = 1 << (j / scale);
      unsigned char mask = 1 << ((j / scale) % FONT_WIDTH);

      // if bit is set in the glyph data, use that color
      // Otherwise, use black (0)
      unsigned int color = (*glyph & mask) ? attr : 0;

      // do not draw the background
      if (color != 0)
      {
        // Draw the pixel
        draw_pixel(x + j, y + i, color);
      }
    }
    // Move to the next line of the glyph data
    // glyph += (i % scale) ? 0 : FONT_BPL;
    if ((i + 1) % scale == 0)
    {
      glyph += FONT_BPL;
    }
  }
}

/**
 * Draws a string of text at the specified position
 *
 * @param x Starting x-coordinate
 * @param y Starting y-coordinate
 * @param s string to draw
 * @param attr the color to use for text
 * @param scale
 */
void draw_string(int x, int y, char *s, unsigned int attr, int scale)
{
  // Go each character of the given string
  while (*s)
  {
    if (*s == '\r')
    {
      // Carriage return - Move to the start of the line
      x = 0;
    }
    else if (*s == '\n')
    {
      // newline - go to the next line
      x = 0;
      y += (FONT_HEIGHT * scale);
    }
    else
    {
      // Draw the current character
      draw_char(*s, x, y, attr, scale);

      // Move to the next charater position
      x += (FONT_WIDTH * scale);
    }
    // Move to the next character in string
    s++;
  }
}
