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
    button_init(240,300,300,85,0, START);
    button_init(240,400, 300,85, 0, QUIT );
    while (1)
    {
        

    }
    
}