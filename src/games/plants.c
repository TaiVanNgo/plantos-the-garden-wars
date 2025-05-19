#include "../../include/plants.h"

void draw_sunflower(int x, int y)
{
  draw_image(sunflower_happy, x, y, PLANT_WIDTH, PLANT_HEIGHT, 0);
}

void draw_peashooter(int x, int y)
{
  draw_image(peashooter, x, y, PLANT_WIDTH, PLANT_HEIGHT, 0);
}

void draw_bullet_green(int x, int y)
{
  draw_image(bullet_green, x, y, BULLET_WIDTH, BULLET_HEIGHT, 0);
}