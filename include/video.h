#ifndef VIDEO_H
#define VIDEO_H

#include "../assets/backgrounds/background.h"
#include "framebf.h"
#include "frames.h"
#include "utils.h"

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
void display_team_members(int show_bg);
#endif // VIDEO_H
