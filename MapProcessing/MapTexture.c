#include "MapTexture.h"
#include <err.h>

SDL_Surface* map_create_texture(SDL_Surface *image, int **river, int **trail,
    int **road_major, int **road_minor, int w, int h)
{
  SDL_LockSurface(image);
  for(int i = 0; i < w; i++)
  {
    for(int j = 0; j < h; j++)
    {
      if(river[i][j] == 1)
      {
        BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 139)));
      }
      if(trail[i][j] == 1)
      {
        BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 165, 42, 42)));
      }
      if(road_major[i][j] == 1)
      {
        BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 255, 90)));
      }
      if(road_minor[i][j] == 1)
      {
        BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 255, 255)));
      }
    }
  }
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, "Pictures/Results/texture.bmp");
  return image;
}
