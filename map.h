#ifndef MAP_H
#define MAP_H MAP_H

#include <libplayerc/playerc.h>
#include <opencv/highgui.h>

#define MAP_SIZE_X 500
#define MAP_SIZE_Y 500
#define MAP_OFFS_X 250
#define MAP_OFFS_Y 250
#define MAP_SCALE 30.0

int map_draw(playerc_ranger_t *ranger, playerc_position2d_t *pos);
int map_shutdown(void);

int map_iswall(double x, double y);

#endif

