// -----------------------------------mbox.c -------------------------------------
#include "mbox.h"


/* Mailbox Data Buffer (each element is 32-bit)*/
/*
 * The keyword attribute allows you to specify special attributes
 *
 * The aligned(N) attribute aligns the current data item on an address
 * which is a multiple of N, by inserting padding bytes before the data item
 *
 * __attribute__((aligned(16)) : allocate the variable on a 16-byte boundary.
 * *
 * We must ensure that our our buffer is located at a 16 byte aligned address,
 * so only the high 28 bits contain the address
 * (last 4 bits is ZERO due to 16 byte alignment)
 *
 */
volatile unsigned int __attribute__((aligned(16))) mBuf[36];

/**
 * Read from the mailbox
 */
uint32_t mailbox_read(unsigned char channel)
{
    // Receiving message is buffer_addr & channel number
    uint32_t res;
    // Make sure that the message is from the right channel
    do
    {
        // Make sure there is mail to receive
        do
        {
            asm volatile("nop");
        } while (MBOX0_STATUS & MBOX_EMPTY);
        // Get the message
        res = MBOX0_READ;
    } while ((res & 0xF) != channel);

    return res;
}

/**
 * Write to the mailbox
 */
void mailbox_send(uint32_t msg, unsigned char channel)
{
    // Sending message is buffer_addr & channel number
    //  Make sure you can send mail
    do
    {
        asm volatile("nop");
    } while (MBOX1_STATUS & MBOX_FULL);
    // send the message
    MBOX1_WRITE = msg;
}

/**
 * Make a mailbox call. Returns 0 on failure, non-zero on success
 */
int mbox_call(unsigned int buffer_addr, unsigned char channel)
{
    // Check Buffer Address
    uart_puts("Buffer Address: ");
    uart_hex(buffer_addr);
    uart_sendc('\n');

    // Prepare Data (address of Message Buffer)
    unsigned int msg = (buffer_addr & ~0xF) | (channel & 0xF);
    mailbox_send(msg, channel);

    /* now wait for the response */
    /* is it a response to our message (same address)? */
    if (msg == mailbox_read(channel))
    {
        /* is it a valid successful response (Response Code) ? */
        if (mBuf[1] == MBOX_RESPONSE)
            uart_puts("Got successful response \n");

        return (mBuf[1] == MBOX_RESPONSE);
    }

    return 0;
}

/**
 * Get the board revision using the mailbox property tags.
 *
 * @return The board revision as a 32-bit unsigned integer, or 0 on failure.
 */
uint32_t get_board_revision(void)
{
    // Prepare the mailbox buffer
    mBuf[0] = 7 * 4;         // Buffer size in bytes (7 words * 4 bytes)
    mBuf[1] = MBOX_REQUEST;  // Request code (0 initially)
    mBuf[2] = 0x00010002;    // Tag: Get board revision
    mBuf[3] = 4;             // Value buffer size (4 bytes for u32 response)
    mBuf[4] = 0;             // Request code = 0
    mBuf[5] = 0;             // Value buffer (will hold the board revision)
    mBuf[6] = MBOX_TAG_LAST; // End tag (0)

    // Make the mailbox call
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {
        // Return the board revision from mBuf[5]
        return mBuf[5];
    }

    // Return 0 on failure
    return 0;
}

/**
 * @brief Parse and display board information based on revision code
 * 
 * @param revision Board revision code
 */
void print_board_info(uint32_t revision)
{
    uart_puts("Board Information: ");
    
    // Parse revision code to determine board model
    if (revision & (1 << 23))
    {
        // New-style revision code (bit 23 is set)
        uint32_t model = (revision & 0xFF0) >> 4;
        uint32_t pcb_rev = (revision & 0xF);
        uint32_t memory = (revision & 0x700000) >> 20;
        uint32_t manufacturer = (revision & 0xF0000) >> 16;
        
        // Display board model
        uart_puts("Raspberry Pi ");
        switch (model)
        {
            case 0: uart_puts("A"); break;
            case 1: uart_puts("B"); break;
            case 2: uart_puts("A+"); break;
            case 3: uart_puts("B+"); break;
            case 4: uart_puts("2B"); break;
            case 6: uart_puts("CM1"); break;
            case 8: uart_puts("3B"); break;
            case 9: uart_puts("Zero"); break;
            case 10: uart_puts("CM3"); break;
            case 12: uart_puts("Zero W"); break;
            case 13: uart_puts("3B+"); break;
            case 14: uart_puts("3A+"); break;
            case 16: uart_puts("CM3+"); break;
            case 17: uart_puts("4B"); break;
            default: uart_puts("Unknown"); break;
        }
        
        // Display memory size
        uart_puts(", Memory: ");
        switch (memory)
        {
            case 0: uart_puts("256MB"); break;
            case 1: uart_puts("512MB"); break;
            case 2: uart_puts("1GB"); break;
            case 3: uart_puts("2GB"); break;
            case 4: uart_puts("4GB"); break;
            case 5: uart_puts("8GB"); break;
            default: uart_puts("Unknown"); break;
        }
        
        // Display manufacturer
        uart_puts(", Manufacturer: ");
        switch (manufacturer)
        {
            case 0: uart_puts("Sony UK"); break;
            case 1: uart_puts("Egoman"); break;
            case 2: uart_puts("Embest"); break;
            case 3: uart_puts("Sony Japan"); break;
            case 4: uart_puts("Embest"); break;
            case 5: uart_puts("Stadium"); break;
            default: uart_puts("Unknown"); break;
        }
        
        // Display PCB revision
        uart_puts(", PCB Rev: ");
        uart_dec(pcb_rev);
    }
    else
    {
        // Old-style revision code
        switch (revision)
        {
            case 0x2: uart_puts("Raspberry Pi B Rev 1.0 (256MB)"); break;
            case 0x3: uart_puts("Raspberry Pi B Rev 1.0 + ECN0001 (256MB)"); break;
            case 0x4: uart_puts("Raspberry Pi B Rev 2.0 (256MB)"); break;
            case 0x5: uart_puts("Raspberry Pi B Rev 2.0 (256MB)"); break;
            case 0x6: uart_puts("Raspberry Pi B Rev 2.0 (256MB)"); break;
            case 0x7: uart_puts("Raspberry Pi A (256MB)"); break;
            case 0x8: uart_puts("Raspberry Pi A (256MB)"); break;
            case 0x9: uart_puts("Raspberry Pi A (256MB)"); break;
            case 0xd: uart_puts("Raspberry Pi B Rev 2.0 (512MB)"); break;
            case 0xe: uart_puts("Raspberry Pi B Rev 2.0 (512MB)"); break;
            case 0xf: uart_puts("Raspberry Pi B Rev 2.0 (512MB)"); break;
            case 0x10: uart_puts("Raspberry Pi B+ (512MB)"); break;
            case 0x11: uart_puts("Raspberry Pi Compute Module (512MB)"); break;
            case 0x12: uart_puts("Raspberry Pi A+ (256MB)"); break;
            default: uart_puts("Unknown Raspberry Pi Model"); break;
        }
    }
}

/**
 * Get the MAC address using the mailbox property tags.
 *
 * @param mac Buffer to store the 6-byte MAC address
 * @return 1 on success, 0 on failure
 */
uint32_t get_mac_address(unsigned char mac[6])
{
    // Prepare mailbox buffer for MAC address request
    mBuf[0] = 8 * 4;                  // Buffer size in bytes (8 words * 4 bytes)
    mBuf[1] = MBOX_REQUEST;           // Request code
    mBuf[2] = 0x00010003;             // Tag: Get MAC address
    mBuf[3] = 6;                      // Value buffer size (6 bytes for MAC)
    mBuf[4] = 0;                      // Request code = 0
    mBuf[5] = 0;                      // Value buffer (will hold the MAC address, first part)
    mBuf[6] = 0;                      // Value buffer (will hold the MAC address, second part)
    mBuf[7] = MBOX_TAG_LAST;          // End tag
    
    if (mbox_call(ADDR(mBuf), MBOX_CH_PROP))
    {
        // Extract MAC address bytes from response
        mac[0] = (mBuf[5] >> 0) & 0xFF;
        mac[1] = (mBuf[5] >> 8) & 0xFF;
        mac[2] = (mBuf[5] >> 16) & 0xFF;
        mac[3] = (mBuf[5] >> 24) & 0xFF;
        mac[4] = (mBuf[6] >> 0) & 0xFF;
        mac[5] = (mBuf[6] >> 8) & 0xFF;
        
        return 1; // Success
    }
    
    return 0; // Failure
}