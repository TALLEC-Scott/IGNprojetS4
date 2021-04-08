#ifndef DRAWING_TOOLS_H
#define DRAWING_TOOLS_H
#include <stdio.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glut.h>

void Draw_Points(int **bp, SDL_Surface *image);
void free_bm(int **bp, SDL_Surface *image);


#endif