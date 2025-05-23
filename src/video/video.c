#include "../include/video.h"
#include "../include/framebf.h"
#include "../include/utils.h"
#include "../assets/backgrounds/background.h"

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

	video->total_frames = 21;
	video->current_frame = 0;
}

const unsigned int *video_get_current_frame(Video *video)
{
	return video->frames[video->current_frame];
}

void display_team_members(int show_bg)
{
	// if show background
	if (show_bg)
	{
		draw_image(BACKGROUND, 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT, 0);
	}

	// Title
	draw_string(260, 20, "EMBEDDED SYSTEMS", RED, 2);
	draw_string(150, 50, "OPERATING SYSTEMS AND INTERFACING", RED, 2);

	// Member 1

	draw_circle(160, 80, 8, WHITE, 1);
	draw_string(200, 75, "NGUYEN PHAM ANH THU (S3926793)", WHITE, 2);

	// Member 2
	draw_circle(160, 100, 8, BRIGHT_BLUE, 1);
	draw_string(200, 95, "HUYNH TAN PHAT (S3929218)", BRIGHT_BLUE, 2);

	// Member 3
	draw_circle(160, 120, 8, SALMON_RED, 1);
	draw_string(200, 115, "HUYNH THAI DUONG (S3978955)", SALMON_RED, 2);

	// Member 4
	draw_circle(160, 140, 8, THISTLE, 1);
	draw_string(200, 135, "NGO VAN TAI (S3974892)", THISTLE, 2);

	// Pulse circle animation
	int pulse = 0;
	int pulse_dir = 1;

	while (1)
	{
		// Pulse animation
		if (pulse_dir > 0)
		{
			pulse++;
			if (pulse >= 10)
				pulse_dir = -1;
		}
		else
		{
			pulse--;
			if (pulse <= 4)
				pulse_dir = 1;
		}
		// Clear previous left circle
		draw_circle(210, 20, 15, PULSE_CIRCLE_COLOR, 1);
		// Draw new pulse left circle
		draw_circle(210, 20, pulse, WHITE, 1);

		// Clear previous right circle
		draw_circle(550, 20, 15, PULSE_CIRCLE_COLOR, 1);
		// Draw new pulse right circle
		draw_circle(550, 20, pulse, WHITE, 1);

		// Process UART input
		char c = getUart();

		if (c == 'q')
		{
			uart_puts("Exit displaying team member!\n");
			clear_screen();
			return;
		}

		// Short delay for animation
		delay_ms(500);
	}
}

void video_next_frame(Video *video)
{
	video->current_frame = (video->current_frame + 1) % video->total_frames;
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
		draw_image(frame, pos_x, pos_y, FRAME_WIDTH, FRAME_HEIGHT, 0);

		// Move to the next frame
		video_next_frame(video);

		// Add a delay between frames (e.g., 600ms)
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