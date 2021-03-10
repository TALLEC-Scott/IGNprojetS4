#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>

Uint32 BMP_Get_Pixel(SDL_Surface *image, int x, int y);
void BMP_Put_Pixel(SDL_Surface *image, int x, int y, Uint32 pixel);
SDL_Surface* BMP_To_BW(SDL_Surface *image);
void BMP_Test(SDL_Surface *image, int **tab);
Uint8 Max(Uint8 r, Uint8 g, Uint8 b);
Uint8 Min(Uint8 r, Uint8 g, Uint8 b);
void RGB_To_HSV(double r, double g, double b, double array[3]);
#endif