#include "MapFilterColor.h"

void BMP_Filter(SDL_Surface *image)
{
    Uint8 h, s, v;
    for(int i = 0; i < image->w; i++)
    {
        for(int j = 0; j < image->h; j++)
        {
            Uint32 pixel = BMP_Get_Pixel(imagem i, j);
            Uint8 r, g, b;
            Uint8 r1, g1, b1;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            r1 = r / 255;
            g1 = g / 255;
            b1 = b / 255;
            max = Max(r1, g1, b1);
            min = Min(r1, g1, b1);


            v = max;
        }
    }
}

