#ifndef VIDEO_H
#define VIDEO_H

#include "framebf.h"
#include "utils.h"
#include "../assets/backgrounds/background.h"

#define FRAME_WIDTH 640
#define FRAME_HEIGHT 360
#define MAX_FRAMES 30

extern const unsigned int *frame_data[MAX_FRAMES];

// Video structure definition
typedef struct
{
    int current_frame;
    int total_frames;
    const unsigned int **frames;
} Video;

const unsigned int *video_get_current_frame(Video *video);
void video_next_frame(Video *video);
void video_init(Video *video);
void play_video(Video *video, int pos_x, int pos_y, int max_frames);
void wipe_transition();
void video_init(Video *video);
#endif // VIDEO_H