#include <stdlib.h>
#include <stdio.h>
#include "MapColorisation.h"
#include "MapFilterColor.h"
#include "tools.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main()
{
  SDL_Surface *image;
  image = SDL_LoadBMP("Pictures/contour_lines.bmp");

  SDL_Surface *test;
  test = SDL_LoadBMP("Pictures/map_ign.bmp");

  if(image == NULL)
    printf("SDL_LoadBMP image failed: %s\n", SDL_GetError());
  SDL_Surface *image2 =  BMP_To_BW(image);
  if(image2 == NULL)
    printf("SDL_LoadBMP image2 failed: %s\n", SDL_GetError());

  if(SDL_SaveBMP(image2, "Pictures/Results/bwImage.bmp") < 0)
    printf("SDL_SaveBMP failed: %s\n", SDL_GetError());

  double r, g, b = 0;
  r = (double)195;
  g = (double)191;
  b = (double)165;

  double *array = calloc(3, sizeof(double));

  RGB_To_HSV(r, g, b, array);
  printf("H: %f, S: %f, V: %f\n", array[0], array[1], array[2]);
  free(array);
  
  Map_Colorisation(image2, NULL);
  bmp_filter(test);

  SDL_FreeSurface(image2);
  SDL_FreeSurface(test);
}
