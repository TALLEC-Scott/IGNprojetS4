#ifndef MAPCOLORISATION_H
#define MAPCOLORISATION_H

#include "tools.h"
void Map_Colorisation(SDL_Surface *image);
int map_dfs_finder(SDL_Surface *image, int x, int y, int label,
    int **tab, int first);
int map_end(int **tab, int x, int y, int h, int w);

#endif
