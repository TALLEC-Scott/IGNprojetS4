#ifndef MAP_PROCESSING_H
#define MAP_PROCESSING_H

#include "../Tools/tools.h"
void Map_Colorisation(SDL_Surface *image);
void Map_DFS_Fill(SDL_Surface *image, int x, int y, int label,
    int **tab);

#endif
