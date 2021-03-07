#include <stdlib.h>
#include <stdio.h>
#include "MapProcessing/MapColorisation.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int main()
{
  SDL_Surface *image;
  image = SDL_LoadBMP("Pictures/contour_lines.bmp");
  if(image < 0)
    printf("SDL_LoadBMP image failed: %s\n", SDL_GetError());
  SDL_Surface *image2 =  BMP_To_BW(image);
  if(image2 < 0)
    printf("SDL_LoadBMP image2 failed: %s\n", SDL_GetError());

  if(SDL_SaveBMP(image2, "Pictures/Results/bwImage.bmp") < 0)
    printf("SDL_SaveBMP failed: %s\n", SDL_GetError());
  
  Map_Colorisation(image2);
  SDL_FreeSurface(image2);
}
