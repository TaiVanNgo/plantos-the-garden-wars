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
extern const unsigned int FRAME11[];
extern const unsigned int FRAME12[];
extern const unsigned int FRAME13[];
extern const unsigned int FRAME14[];
extern const unsigned int FRAME15[];
extern const unsigned int FRAME16[];
extern const unsigned int FRAME17[];
extern const unsigned int FRAME18[];
extern const unsigned int FRAME19[];
extern const unsigned int FRAME20[];
extern const unsigned int FRAME21[];
extern const unsigned int FRAME22[];
extern const unsigned int FRAME23[];
extern const unsigned int FRAME24[];
extern const unsigned int FRAME25[];
extern const unsigned int FRAME26[];
extern const unsigned int FRAME27[];
extern const unsigned int FRAME28[];
extern const unsigned int FRAME29[];
extern const unsigned int FRAME30[];
extern const unsigned int FRAME31[];
extern const unsigned int FRAME32[];
extern const unsigned int FRAME33[];
extern const unsigned int FRAME34[];
extern const unsigned int FRAME35[];
extern const unsigned int FRAME36[];


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