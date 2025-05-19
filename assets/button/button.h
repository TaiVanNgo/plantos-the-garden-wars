#define BUTTON_NORMAL    0
#define BUTTON_SELECTED  1
#define BUTTON_PRESSED   2
extern const unsigned int START[];
extern const unsigned int QUIT[];
extern const unsigned int ARROW[];

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
void button_handle_keypress(Button* buttons[], int num_buttons, int current_button, char key);
void button_set_state(Button* button, int state);
void button_draw_selection(Button *button);
int button_is_selected(Button* button);