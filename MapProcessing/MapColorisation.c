#include "MapColorisation.h"

void Map_Colorisation(SDL_Surface *image)
{
  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      Uint32 pixel = BMP_Get_Pixel(image, i, j);
      Uint8 r, g, b;
      SDL_GetRGB(pixel, image->format, &r, &g, &b);
      if(r == 0)
      {
        // Black pixel find
      }
    }
  }
}
