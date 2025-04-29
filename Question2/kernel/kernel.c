#include "../uart/uart0.h"
#include "../uart/uart1.h"
#include "mbox.h"
#include "framebf.h"
#include "./video.h"
void delay_ms(unsigned int ms) {
    // Each iteration takes about 4 cycles
    // ARM CPU frequency is typically 1GHz = 1,000,000,000 cycles per second
    // So for 1ms we need 250,000 iterations (1,000,000,000 / (4 * 1000))
    unsigned int count = ms * 250000;
    for(unsigned int i = 0; i < count; i++) {
        asm volatile("nop");  // No operation, prevents optimization
    }
}
void drawImage(const unsigned int pixel_data[], int pos_x, int pos_y, int width, int height){
    for (int i = 0; i < width*height; i++){
        int x = pos_x + (i % width);
        int y = pos_y + (i / width);
        drawPixelARGB32(x, y, pixel_data[i]);
    }
}
void play_video(Video* video, int pos_x, int pos_y) {
    while(1) {
        // Get current frame and draw it
        const unsigned int* frame = video_get_current_frame(video);
        drawImage(frame, pos_x, pos_y, FRAME_WIDTH, FRAME_HEIGHT);
        
        // Move to next frame
        video_next_frame(video);
        
        // Add delay between frames
		delay_ms(300);
    }
}
void main()
{
    // set up serial console
	uart_init();

	// say hello
	uart_puts("\n\nHello World!\n");

	// Initialize frame buffer
	framebf_init();

	Video vid;
	video_init(&vid);


	play_video(&vid, 100, 100);
	// drawImage(FRAME1, 100, 100, 800, 500);


	// echo everything back
	while(1) {
		//read each char
		char c = uart_getc();

		//send back 
		uart_sendc(c);
	}
}