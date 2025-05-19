// #include "../include/uart0.h"
// #include "../include/uart1.h"
// #include "../include/mbox.h"
// #include "../include/framebf.h"
// #include "../include/cmd.h"
// #include "../include/video.h"
// #include "../include/utils.h"
#include "../assets/backgrounds/background.h"
#include "../assets/button/button.h"

void game_init(){
    
    draw_image(BACKGROUND, 0,0, 800,600,0);
    // draw_image(START,0,0, 800,600,0);
    Button startButton; 
    Button endButton;
    button_init(&startButton,240,300,300,85, START);
    button_init( &endButton,240,400, 300,85, QUIT );
    while (1)
    {
        

    }
    
}