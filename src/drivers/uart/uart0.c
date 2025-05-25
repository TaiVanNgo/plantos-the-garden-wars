#include "../../../include/uart0.h"

/**
 * Set baud rate and characteristics and map to GPIO
 */
void uart_init()
{
	unsigned int r;

	/* Turn off UART0 */
	UART0_CR = 0x0;

	/* Setup GPIO pins 14 and 15 */

	/* Set GPIO14 and GPIO15 to be pl011 TX/RX which is ALT0	*/
	r = GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));		// clear bits 17-12 (FSEL15, FSEL14)
	r |= (0b100 << 12) | (0b100 << 15); // Set value 0b100 (select ALT0: TXD0/RXD0)
	GPFSEL1 = r;

	/* enable GPIO 14, 15 */
#ifdef RPI3	   // RPI3
	GPPUD = 0; // No pull up/down control
	// Toogle clock to flush GPIO setup
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	} // waiting 150 cycles
	GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	} // waiting 150 cycles
	GPPUDCLK0 = 0; // flush GPIO setup

#else // RPI4
	r = GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
	GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

	/* Mask all interrupts. */
	UART0_IMSC = 0;

	/* Clear pending interrupts. */
	UART0_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz);
	Integer part register UART0_IBRD  = integer part of Divider
	Fraction part register UART0_FBRD = (Fractional part * 64) + 0.5 */

	// 115200 baud
	UART0_IBRD = 26;
	UART0_FBRD = 3;

	/* Set up the Line Control Register */
	/* Enable FIFO */
	/* Set length to 8 bit */
	/* Defaults for other bit are No parity, 1 stop bit */
	UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;

	/* Enable UART0, receive, and transmit */
	UART0_CR = 0x301; // enable Tx, Rx, FIFO
}

// Reinit uart when changing baudrate
int uart_init_with_baudrate(int baudrate)
{
	uart_puts(baudrate);
	unsigned int r;

	/* Turn off UART0 */
	UART0_CR = 0x0;

	/* Setup GPIO pins 14 and 15 */

	/* Set GPIO14 and GPIO15 to be pl011 TX/RX which is ALT0	*/
	r = GPFSEL1;
	r &= ~((7 << 12) | (7 << 15));		// clear bits 17-12 (FSEL15, FSEL14)
	r |= (0b100 << 12) | (0b100 << 15); // Set value 0b100 (select ALT0: TXD0/RXD0)
	GPFSEL1 = r;

	/* enable GPIO 14, 15 */
#ifdef RPI3	   // RPI3
	GPPUD = 0; // No pull up/down control
	// Toogle clock to flush GPIO setup
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	} // waiting 150 cycles
	GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	} // waiting 150 cycles
	GPPUDCLK0 = 0; // flush GPIO setup

#else // RPI4
	r = GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
	GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

	/* Mask all interrupts. */
	UART0_IMSC = 0;

	/* Clear pending interrupts. */
	UART0_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz);
	Integer part register UART0_IBRD  = integer part of Divider
	Fraction part register UART0_FBRD = (Fractional part * 64) + 0.5 */

	if (set_uart_baudrate(baudrate) != 0)
	{
		return -1;
	}

	/* Set up the Line Control Register */
	/* Enable FIFO */
	/* Set length to 8 bit */
	/* Defaults for other bit are No parity, 1 stop bit */
	UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;

	/* Enable UART0, receive, and transmit */
	UART0_CR = 0x301; // enable Tx, Rx, FIFO

	return 0;
}

/**
 * Send a character
 */
void uart_sendc(char c)
{

	/* Check Flags Register */
	/* And wait until transmitter is not full */
	do
	{
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_TXFF);

	/* Write our data byte out to the data register */
	UART0_DR = c;
}

/**
 * Receive a character
 */
char uart_getc()
{
	char c = 0;

	/* Check Flags Register */
	/* Wait until Receiver is not empty
	 * (at least one byte data in receive fifo)*/
	do
	{
		asm volatile("nop");
	} while (UART0_FR & UART0_FR_RXFE);

	/* read it and return */
	c = (unsigned char)(UART0_DR);

	/* convert carriage return to newline */
	return (c == '\r' ? '\n' : c);
}

/**
 * Display a string
 */
void uart_puts(const char *s)
{
	while (*s)
	{
		/* convert newline to carriage return + newline */
		if (*s == '\n')
			uart_sendc('\r');
		uart_sendc(*s++);
	}
}

void uart_hex(unsigned int num)
{
	uart_puts("0x");
	for (int pos = 28; pos >= 0; pos = pos - 4)
	{

		// Get highest 4-bit nibble
		char digit = (num >> pos) & 0xF;

		/* Convert to ASCII code */
		// 0-9 => '0'-'9', 10-15 => 'A'-'F'
		digit += (digit > 9) ? (-10 + 'A') : '0';
		uart_sendc(digit);
	}
}

/*
**
* Display a value in decimal format
*/
void uart_dec(int num)
{
	// A string to store the digit characters
	char str[33] = {0};

	// Calculate the number of digits
	int len = 1;
	int temp = num;
	while (temp >= 10)
	{
		len++;
		temp = temp / 10;
	}

	// Store into the string and print out
	for (int i = 0; i < len; i++)
	{
		int digit = num % 10; // get last digit
		num = num / 10;		  // remove last digit from the number
		str[len - (i + 1)] = digit + '0';
	}
	str[len] = '\0';

	uart_puts(str);
}

// Function to display 2-digit hex representation for MAC address
void uart_mac_hex(unsigned char num)
{
	// Get highest 4-bit nibble
	char digit = (num >> 4) & 0xF;

	// Convert to ASCII code
	// 0-9 => '0'-'9', 10-15 => 'A'-'F'
	digit += (digit > 9) ? (-10 + 'A') : '0';
	uart_sendc(digit);

	// Get lowest 4-bit nibble
	digit = num & 0xF;

	// Convert to ASCII code
	digit += (digit > 9) ? (-10 + 'A') : '0';
	uart_sendc(digit);
}

unsigned int uart_isReadByteReady()
{
	return (!(UART0_FR & UART0_FR_RXFE));
}

unsigned char getUart()
{
	unsigned char ch = 0;
	if (uart_isReadByteReady())
		ch = uart_getc();
	return ch;
}

void uart_toggle_rts_cts()
{
	/* Check if RTS and CTS are enabled */
	if (UART0_CR & (UART0_CR_RTSEN | UART0_CR_CTSEN))
	{
		/* If enabled, disable RTS and CTS */
		UART0_CR &= ~(UART0_CR_RTSEN | UART0_CR_CTSEN);
		uart_puts("\nRTS/CTS handshaking disabled\n");
	}
	else
	{
		/* If disabled, enable RTS and CTS */
		UART0_CR |= (UART0_CR_RTSEN | UART0_CR_CTSEN);
		uart_puts("\nRTS/CTS handshaking enabled\n");
	}
}

int set_uart_baudrate(int baudrate)
{
	// Check if baudrate is valid
	if (baudrate <= 0)
		return -1; // Invalid baudrate

	// UART clock frequency (48MHz)
	const unsigned int UART_CLOCK = 48000000;

	// Calculate divider using formula: Divider = UART_CLOCK/(16 * Baud)
	float divider = (float)UART_CLOCK / (16.0f * baudrate);

	// Calculate integer and fractional parts
	// UART0_IBRD = integer part of Divider
	// UART0_FBRD = (Fractional part * 64) + 0.5
	unsigned int ibrd = (unsigned int)divider;
	unsigned int fbrd = (unsigned int)((divider - ibrd) * 64.0f + 0.5f);

	// Assign calculated values to registers
	UART0_IBRD = ibrd;
	UART0_FBRD = fbrd;

	return 0; // Success
}

void RTS_CTS_init()
{
	unsigned int r;

	/* Turn off UART0 */
	UART0_CR = 0x0;

	/* NEW: set up UART clock for consistent divisor values
	--> may not work with QEMU, but will work with real board */
	// mBuf[0] = 9*4;
	// mBuf[1] = MBOX_REQUEST;
	// mBuf[2] = MBOX_TAG_SETCLKRATE; // set clock rate
	// mBuf[3] = 12; // Value buffer size in bytes
	// mBuf[4] = 0; // REQUEST CODE = 0
	// mBuf[5] = 2; // clock id: UART clock
	// mBuf[6] = 4000000;     // rate: 4Mhz
	// mBuf[7] = 0;           // clear turbo
	// mBuf[8] = MBOX_TAG_LAST;
	// mbox_call(ADDR(mBuf), MBOX_CH_PROP);

	/* Setup GPIO pins 14 and 15 */

	/* Set GPIO14 and GPIO15 to be pl011 TX/RX which is ALT0	*/
	r = GPFSEL1;

	// RTS/CTS mode
	r &= ~((7 << 18) | (7 << 21));		// clear bits 23-18 (FSEL16, FSEL17)
	r |= (0b111 << 18) | (0b111 << 21); // Set value 0b111 (select ALT3: TXD0/RXD0)

	GPFSEL1 = r;

	/* enable GPIO 14, 15 */
#ifdef RPI3	   // RBP3
	GPPUD = 0; // No pull up/down control
	// Toogle clock to flush GPIO setup
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}								   // waiting 150 cycles
	GPPUDCLK0 = (1 << 14) | (1 << 15); // enable clock for GPIO 14, 15
	r = 150;
	while (r--)
	{
		asm volatile("nop");
	}			   // waiting 150 cycles
	GPPUDCLK0 = 0; // flush GPIO setup

#else // RPI4
	r = GPIO_PUP_PDN_CNTRL_REG0;
	r &= ~((3 << 28) | (3 << 30)); // No resistor is selected for GPIO 14, 15
	GPIO_PUP_PDN_CNTRL_REG0 = r;
#endif

	/* Mask all interrupts. */
	UART0_IMSC = 0;

	/* Clear pending interrupts. */
	UART0_ICR = 0x7FF;

	/* Set integer & fractional part of Baud rate
	Divider = UART_CLOCK/(16 * Baud)
	Default UART_CLOCK = 48MHz (old firmware it was 3MHz);
	Integer part register UART0_IBRD  = integer part of Divider
	Fraction part register UART0_FBRD = (Fractional part * 64) + 0.5 */

	// 115200 baud
	//  UART0_IBRD = 26;
	//  UART0_FBRD = 3;
	//  int *BR = caculate_IBRD(baudrate);

	// NEW: with UART_CLOCK = 4MHz as set by mailbox:
	// 115200 baud
	//  UART0_IBRD = BR[0]; //Integer part
	//  UART0_FBRD = BR[1]; //Fractional part

	/* Set up the Line Control Register */
	/* Enable FIFO */
	/* Set length to 8 bit */
	/* Defaults for other bit are No parity, 1 stop bit */
	UART0_LCRH = UART0_LCRH_FEN | UART0_LCRH_WLEN_8BIT;

	UART0_CR = 0x301;

	/* Enable UART0, receive, and transmit */
	// RTS/CTS mode
	UART0_CR |= (UART0_CR_CTSEN | UART0_CR_RTSEN);
	// enable Tx, Rx, FIFO
}