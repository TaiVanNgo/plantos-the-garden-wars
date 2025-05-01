#include "../include/video.h"
#include "../include/framebf.h"
#include "../include/utils.h"
// Implementation of video functions
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
	video->total_frames = sizeof(video->frames) / sizeof(video->frames[0]); // Update this based on number of frames;
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

void play_video(Video *video, int pos_x, int pos_y)
{
	while (1)
	{
		// Get current frame and draw it
		const unsigned int *frame = video_get_current_frame(video);
		drawImage(frame, pos_x, pos_y, FRAME_WIDTH, FRAME_HEIGHT);

		// Move to next frame
		video_next_frame(video);

		// Add delay between frames
		delay_ms(300);
	}
}