#ifndef MAPREBUILTHOLES_H
#define MAPREBUILTHOLES_H

#include "tools.h"

int is_black(SDL_Surface *image, int x, int y);
int is_in_filter(SDL_Surface *image, int x, int y);
void thinning(SDL_Surface *image);
void clean_point(SDL_Surface *image);
void rec_moore(SDL_Surface *image, int **mark, int clock[8][2],
    int x, int y, int i);
void moore(SDL_Surface *image);

#endif
