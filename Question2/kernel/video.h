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
typedef struct {
    const unsigned int* frames[10];
    int current_frame;
    int total_frames;
} Video;
#define FRAME_WIDTH 800
#define FRAME_HEIGHT 500
#define FRAME_DELAY 100000 
static inline const unsigned int* video_get_current_frame(Video* video) {
    return video->frames[video->current_frame];
}

static inline void video_next_frame(Video* video) {
    video->current_frame = (video->current_frame + 1) % video->total_frames;
}


static inline void video_init(Video* video) {
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
    // Add more frames as needed
    video->total_frames = sizeof(video->frames) / sizeof(video->frames[0]);  // Update this based on number of frames
    video->current_frame = 0;
}