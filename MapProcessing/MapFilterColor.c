#include "MapFilterColor.h"

void BMP_Filter(SDL_Surface *image)
{
    SDL_LockSurface(image);
    Uint8 h, s, v;
    for(int i = 0; i < image->w; i++)
    {
        for(int j = 0; j < image->h; j++)
        {
            Uint32 pixel = BMP_Get_Pixel(image, i, j);
            Uint8 r, g, b;
            double h, s, v;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);
            double *array = calloc(3, sizeof(double));
            RGB_To_HSV(r, g, b, array);
            h = array[0];
            s = array[1];
            v = array[2];
            //printf("H: %f, S: %f, V: %f\n", h, s, v);

            double temp_h = h / (double)100;

            // Black colors
            if(v < 0.5)
            {
              continue;
            }
            
            // White colors
            if(s < 0.2 && v > 0.8)
            {
              continue;
            }
            printf("%f\n", temp_h);
            
            // Brown colors
            if(temp_h > (double)0 && temp_h < 0.5)
            {
              
              BMP_Put_Pixel(image, i, j,
                  (SDL_MapRGB(image->format, 255, 0, 0)));
            }

        }
    }
    SDL_UnlockSurface(image);
    SDL_SaveBMP(image, "Pictures/Results/test.bmp");

}

