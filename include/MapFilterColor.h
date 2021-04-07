#ifndef MAPFILTERCOLOR_H
#define MAPFILTERCOLOR_H

#include "tools.h"

void bmp_filter(SDL_Surface *image, int r, int g, int b, int **bp);
void filter(SDL_Surface *image, int **array_topo, int **array_river,
    int **array_road, int r1, int g1, int b1, int **bp);

#endif
