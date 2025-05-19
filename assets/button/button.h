#define BUTTON_NORMAL    0
#define BUTTON_SELECTED  1
#define BUTTON_PRESSED   2
extern const unsigned int START[];
extern const unsigned int QUIT[];


// Button structure
typedef struct {
    int x;             
    int y;              
    int width;          
    int height;         
    int state;          
	unsigned int *pixel_data; 
    void (*callback)(); 
} Button;


button_init(Button* button, int x, int y, int width, int height, const unsigned int pixel_data[]);