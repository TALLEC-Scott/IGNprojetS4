#ifndef MAPCOLORISATION_H
#define MAPCOLORISATION_H

#include "tools.h"
void Map_Colorisation(SDL_Surface *image);
void Map_DFS_Fill(SDL_Surface *image, int x, int y, int label,
    int **tab);

#endif
