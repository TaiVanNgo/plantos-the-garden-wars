
extern const unsigned int START[];
extern const unsigned int QUIT[];


// Button structure
typedef struct {
    int x;             
    int y;              
    int width;          
    int height;         
    int state;          
	int *pixel_data; 
    void (*callback)(); 
} Button;


button_init(int x, int y, int width, int height, int state, int *pixel_data);