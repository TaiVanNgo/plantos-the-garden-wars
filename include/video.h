#ifndef VIDEO_H
#define VIDEO_H

#define FRAME_WIDTH 800
#define FRAME_HEIGHT 500
#define FRAME_DELAY 100000

// Define in frames.c
extern const unsigned int FRAME1[];
extern const unsigned int FRAME2[];
extern const unsigned int FRAME3[];
extern const unsigned int FRAME4[];
extern const unsigned int FRAME5[];
extern const unsigned int FRAME6[];
extern const unsigned int FRAME7[];
extern const unsigned int FRAME8[];
extern const unsigned int FRAME9[];
extern const unsigned int FRAME10[];

// Video structure definition
typedef struct
{
    const unsigned int *frames[10];
    int current_frame;
    int total_frames;
} Video;

const unsigned int *video_get_current_frame(Video *video);
void video_next_frame(Video *video);
void video_init(Video *video);
void play_video(Video *video, int pos_x, int pos_y);
#endif // VIDEO_H