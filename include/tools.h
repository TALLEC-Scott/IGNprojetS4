#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include "MapRebuiltHoles.h"

Uint32 BMP_Get_Pixel(SDL_Surface *image, int x, int y);
void BMP_Put_Pixel(SDL_Surface *image, int x, int y, Uint32 pixel);
void bmp_create(SDL_Surface *image, int **array, char *name);
void BMP_Test(SDL_Surface *image, int **tab);
double Max(double r, double g, double b);
double Min(double r, double g, double b);
void RGB_To_HSV(double r, double g, double b, double array[3]);
void bmp_white(SDL_Surface *image);
void bmp_test2(SDL_Surface *image, int **tab);
int is_black(SDL_Surface *image, int x, int y);
size_t len_array_int(int *array);
void bmp_test3(SDL_Surface *image, int **tab, int **h);
#endif
