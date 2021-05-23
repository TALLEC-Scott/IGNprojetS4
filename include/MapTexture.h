#ifndef MAPTEXTURE_H
#define MAPTEXTURE_H

#include "tools.h"
#include <err.h>

SDL_Surface* map_create_texture(SDL_Surface *image, int **river, int **trail,
    int **road_major, int **road_minor, int w, int h);

#endif
