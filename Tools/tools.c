#include "tools.h"
#include <string.h>

// Get pixel from coords x, y in BMP file image
Uint32 BMP_Get_Pixel(SDL_Surface *image, int x, int y)
{
  int bpp = image->format->BytesPerPixel;
  Uint8 *p = (Uint8 *)image->pixels+y*image->pitch+x*bpp;
  switch(bpp)
  {
      case 1:
          return *p;

      case 2:
          return *(Uint16 *)p;

      case 3:
          if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
              return p[0] << 16 | p[1] << 8 | p[2];
          else
              return p[0] | p[1] << 8 | p[2] << 16;

      case 4:
          return *(Uint32 *)p;

      default:
          return 0;
  }
}

// Put pixel "pixel" in coords x, y in BMP file image
void BMP_Put_Pixel(SDL_Surface *image, int x, int y, Uint32 pixel)
{
  int bpp = image->format->BytesPerPixel;
  Uint8 *p = (Uint8 *)image->pixels + y * image->pitch + x * bpp;

  switch(bpp) {
  case 1:
      *p = pixel;
      break;

  case 2:
        *(Uint16 *)p = pixel;
      break;

  case 3:
      if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
          p[0] = (pixel >> 16) & 0xff;
           p[1] = (pixel >> 8) & 0xff;
          p[2] = pixel & 0xff;
      } else {
          p[0] = pixel & 0xff;
          p[1] = (pixel >> 8) & 0xff;
          p[2] = (pixel >> 16) & 0xff;
      }
      break;

  case 4:
      *(Uint32 *)p = pixel;
      break;
  }
}

// Convert BMP file image into black and white BMP
SDL_Surface* BMP_To_BW(SDL_Surface *image)
{
  SDL_LockSurface(image);

  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      Uint8 r, g, b;

      Uint32 pixel = BMP_Get_Pixel(image, i, j);
      SDL_GetRGB(pixel, image->format, &r, &g, &b);
      Uint32 pixel_grey = (r+g+b) / 3;

      if (pixel_grey < 128)
      {
        BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 0)));
      }
      else
      {
        BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 255, 255)));
      }
    }
  }
  SDL_UnlockSurface(image);
  return image;
}

void BMP_Test(SDL_Surface *image, int **tab)
{
  SDL_LockSurface(image);
  int tab2[5][3] = {
    {255, 0, 0},
    {0, 0, 255},
    {0, 255, 0},
    {0, 255, 255},
    {255, 255, 0}
  };
  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      int label = tab[i][j];
      if(label != 0)
      {
         while(label >= 5)
         {
           label -= 5;
         }
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, tab2[label][0],
               tab2[label][1], tab2[label][2])));
      }
    }
  }
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, "Pictures/Results/result.bmp");

}

Uint8 Max(Uint8 r, Uint8 g, Uint8 b)
{
    if(r >= g && r >= b)
        return r;
    if(b >= r && b >= g)
        return b;
    if(g >= r && g >= b)
        return g;
    return 0;
}

Uint8 Min(Uint8 r, Uint8 g, Uint8 b)
{
    if(r <= g && r <= b)
        return r;
    if(b <= r && b <= g)
        return b;
    if(g <= r && g <= b)
        return g;
    return 0;
}

double* RGB_To_HSV(double r, double g, double b)
{
    double r1, g1, b1, max, min, delta, h, s, v;

    r1 = r / (double)255;
    g1 = g / (double)255;
    b1 = b / (double)255;

    max = Max(r1, g1, b1);
    min = Min(r1, g1, b1);
    delta = max - min;

    v = max;
    if(max == (double)0)
    {
        s = (double)0;
    }
    else
    {
        s = delta / max;
    }

    if(delta == 0)
    {
        h = (double)0;
    }
    else
    {
        switch(max)
        {
            case r1:
                h = (double)60 * (((g1 - b1) / delta)%6);
                break;
            case g1:
                h = (double)60 * (((b1 - r1) / delta) + 2);
                break;
            case b1:
                h = (double)60 * (((r1 - g1) / delta) + 4);
                break;
            default:
                break;
        }
    }

    double *array = {h, s, v};
    return array;
}
