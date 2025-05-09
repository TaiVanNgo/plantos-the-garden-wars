#include "../include/video.h"
#include "../include/framebf.h"
#include "../include/utils.h"
#include "../include/background.h"

void video_init(Video *video)
{
	video->frames[0] = FRAME1;
	video->frames[1] = FRAME2;
	video->frames[2] = FRAME3;
	video->frames[3] = FRAME4;
	video->frames[4] = FRAME5;
	video->frames[5] = FRAME6;
	video->frames[6] = FRAME7;
	video->frames[7] = FRAME8;
	video->frames[8] = FRAME9;
	video->frames[9] = FRAME10;
	video->frames[10] = FRAME11;
	video->frames[11] = FRAME12;
	video->frames[12] = FRAME13;
	video->frames[13] = FRAME14;
	video->frames[14] = FRAME15;
	video->frames[15] = FRAME16;
	video->frames[16] = FRAME17;
	video->frames[17] = FRAME18;
	video->frames[18] = FRAME19;
	video->frames[19] = FRAME20;
	video->frames[20] = FRAME21;
	video->frames[21] = FRAME22;
	video->frames[22] = FRAME23;
	video->frames[23] = FRAME24;
	video->frames[24] = FRAME25;
	video->frames[25] = FRAME26;
	video->frames[26] = FRAME27;
	video->frames[27] = FRAME28;
	video->frames[28] = FRAME29;
	video->frames[29] = FRAME30;
	video->frames[30] = FRAME31;
	video->frames[31] = FRAME32;
	video->frames[32] = FRAME33;
	video->frames[33] = FRAME34;
	video->frames[34] = FRAME35;
	video->frames[35] = FRAME36;
	video->total_frames = 36;
	video->current_frame = 0;
}

const unsigned int *video_get_current_frame(Video *video)
{
	return video->frames[video->current_frame];
}

void video_next_frame(Video *video)
{
	video->current_frame = (video->current_frame + 1) % video->total_frames;
}

void render_next_frame(Video *video, int pos_x, int pos_y)
{
	delay_ms(700);
	const unsigned int *frame = video_get_current_frame(video);
	draw_image(frame, pos_x, pos_y, FRAME_WIDTH, FRAME_HEIGHT);

	video_next_frame(video);
}

void play_video(Video *video, int pos_x, int pos_y, int max_frames)
{
	char total_frames_str[12];
	for (int i = 0; i < video->total_frames; i++)
	{
		// Convert the total_frames integer to string
		int_to_str(video->total_frames, total_frames_str);

		uart_puts("Playing frame: ");
		char current_frame_str[12];
		int_to_str(i, current_frame_str);
		uart_puts(current_frame_str);
		uart_puts("\n");
		// Get the current frame
		const unsigned int *frame = video_get_current_frame(video);

		// Draw the current frame
		draw_image(frame, pos_x, pos_y, FRAME_WIDTH, FRAME_HEIGHT);

		// Move to the next frame
		video_next_frame(video);

		// Add a delay between frames (e.g., 700ms)
		delay_ms(600);
	}

	// After all frames have been displayed, print completion message
	uart_puts("Video playback completed!\n");
}

void wipe_transition()
{
	for (int y = 0; y < BACKGROUND_HEIGHT; y++)
	{
		// Only draw the current row that's being revealed
		for (int x = 0; x < BACKGROUND_WIDTH; x++)
		{
			// Calculate the index in the background image array
			int index = y * BACKGROUND_WIDTH + x;

			// Draw just this pixel at the correct location
			draw_pixel(x, y, BACKGROUND[index]);
		}
		// Short delay to control the animation speed
		delay_ms(20);
	}
}