#ifndef MAPFILTERCOLOR_H
#define MAPFILTERCOLOR_H

#include "tools.h"

void bmp_filter(SDL_Surface *image, int r, int g, int b,
    int r1, int g1, int b1, int r2, int g2, int b2
    ,int **bp, int **tab, int **h, int **road_major, int **road_minor,
    int **river, int **trail, int **name);

void rec_sampling(int ***arr, int w, int h, int **mark, int ngh[8][2], int count, int x, int y);
void sampling(int ***arr, int w, int h);

void filter(SDL_Surface *image, int **array_topo, int **array_river,
    int **array_road_major, int **array_road_minor, int **array_trail, int r1,
    int g1, int b1, int r2, int g2, int b2, int r3, int g3, int b3, 
    int **bp, int **tab, int **h, int **name);


#endif
