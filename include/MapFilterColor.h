#ifndef MAPFILTERCOLOR_H
#define MAPFILTERCOLOR_H

#include "tools.h"

void bmp_filter(SDL_Surface *image);
void filter(SDL_Surface *image, int **array_topo, int **array_river,
    int **array_road);
void bmp_create(SDL_Surface *image, int **array, char *name);

#endif
