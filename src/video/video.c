#include "../../include/video.h"
#include "../../include/frames.h"

const unsigned int *frame_data[MAX_FRAMES] = {
		FRAME1,
		FRAME2,
		FRAME3,
		FRAME4,
		FRAME5,
		FRAME6,
		FRAME7,
		FRAME8,
		FRAME9,
		FRAME10,
		FRAME11,
		FRAME12,
		FRAME13,
		FRAME14,
		FRAME15,
		FRAME16,
		FRAME17,
		FRAME18,
		FRAME19,
		FRAME20,
		FRAME21,
};

const unsigned int *video_get_current_frame(Video *video)
{
	return video->frames[video->current_frame];
}

void video_next_frame(Video *video)
{
	video->current_frame = (video->current_frame + 1) % video->total_frames;
}

void video_init(Video *video)
{
	video->current_frame = 0;
	video->total_frames = 21;
	video->frames = frame_data;
}

void play_video(Video *video, int pos_x, int pos_y, int max_frame)
{
	// keep track previous frame, only update changed pixels
	static unsigned int prev_pixels[FRAME_WIDTH * FRAME_HEIGHT] = {0};
	unsigned int frame_count = (max_frame < video->total_frames) ? max_frame : video->total_frames;

	// reset the first frame
	video->current_frame = 0;

	// Initialize prev_pixels with values that will force a complete update for first frame
	for (int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; i++)
	{
		prev_pixels[i] = 0xFFFFFFFF;
	}

	for (int i = 0; i < frame_count; i++)
	{
		set_wait_timer(1, 50);

		uart_puts("Playing frame: ");
		char current_frame_str[12];
		int_to_str(i, current_frame_str);
		uart_puts(current_frame_str);
		uart_puts("\n");

		// Get current frame data
		const unsigned int *frame = video_get_current_frame(video);

		// Only update pixels that have changed
		for (int y = 0; y < FRAME_HEIGHT; y++)
		{
			for (int x = 0; x < FRAME_WIDTH; x++)
			{
				int idx = y * FRAME_WIDTH + x;
				if (prev_pixels[idx] != frame[idx])
				{
					draw_pixel(pos_x + x, pos_y + y, frame[idx]);
					prev_pixels[idx] = frame[idx];
				}
			}
		}

		video_next_frame(video);

		set_wait_timer(0, 0);
	}
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

	// Pulse circle animation - do it once
	int pulse = 0;
	int pulse_dir = 1;
	
	// Do one complete pulse cycle
	while (pulse < 10)
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

		// Short delay for animation
		delay_ms(500);
	}
}

// void play_video(Video *video, int pos_x, int pox_y, int max_frames)
// {
// 	static unsigned int prev_frame[FRAME_WIDTH * FRAME_HEIGHT] = {0};

// 	for (int i = 0; i < video->total_frames && i < max_frames; i++)
// 	{
// 		// Get current frame
// 		const unsigned int *frame = video_get_current_frame(video);

// 		uart_puts("Playing frame: ");
// 		char current_frame_str[12];
// 		int_to_str(i, current_frame_str);
// 		uart_puts(current_frame_str);
// 		uart_puts("\n");

// 		for (int y = 0; y < FRAME_HEIGHT; y++)
// 		{
// 			for (int x = 0; x < FRAME_WIDTH; x++)
// 			{
// 				int idx = y * FRAME_WIDTH + x;
// 				if (prev_frame[idx] != frame[idx])
// 				{
// 					draw_pixel(pos_x + x, pox_y + y, frame[idx]);
// 					prev_frame[idx] = frame[idx];
// 				}
// 			}
// 		}

// 		// Move to next frame
// 		video_next_frame(video);
// 		// delay between frames
// 		delay_ms(100);
// 	}
// }

// void play_video(Video *video, int pos_x, int pos_y, int max_frames)
// {
// 	char total_frames_str[12];
// 	for (int i = 0; i < video->total_frames; i++)
// 	{
// 		// Convert the total_frames integer to string
// 		int_to_str(video->total_frames, total_frames_str);

// 		uart_puts("Playing frame: ");
// 		char current_frame_str[12];
// 		int_to_str(i, current_frame_str);
// 		uart_puts(current_frame_str);
// 		uart_puts("\n");
// 		// Get the current frame
// 		const unsigned int *frame = video_get_current_frame(video);

// 		// Draw the current frame
// 		draw_image(frame, pos_x, pos_y, FRAME_WIDTH, FRAME_HEIGHT, 0);

// 		// Move to the next frame
// 		video_next_frame(video);

// 		// Add a delay between frames (e.g., 700ms)
// 		delay_ms(100);
// 	}

// 	// After all frames have been displayed, print completion message
// 	uart_puts("Video playback completed!\n");
// }

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