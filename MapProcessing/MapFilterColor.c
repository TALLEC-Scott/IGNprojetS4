#include "MapFilterColor.h"

// bmp_filter initializes array for stores all the informations about
// the ign map
void bmp_filter(SDL_Surface *image, int r, int g, int b, int **bp)
{

  int **topo = NULL;
  topo = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  {
    topo[i] = (int*)calloc(image->h, sizeof(int*));
  }

  int **river = NULL;
  river = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  {
    river[i] = (int*)calloc(image->h, sizeof(int*));
  }

  int **road = NULL;
  road = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  {
    road[i] = (int*)calloc(image->h, sizeof(int*));
  }

  filter(image, topo, river, road, r, g, b, bp);

  // free
  for(int i = 0; i < image->w; i++)
  {
    free(topo[i]);
    free(river[i]);
    free(road[i]);
  }

  free(topo);
  free(river);
  free(road);
}

// filter Converts all rgb values to HSV in order to filters colors
void filter(SDL_Surface *image, int **array_topo, int **array_river,
    int **array_road, int r1, int g1, int b1, int **bp)
{
    double *array = calloc(3, sizeof(double));
    SDL_LockSurface(image);
    printf("r %i\n", r1);
    for(int i = 0; i < image->w; i++)
    {
        for(int j = 0; j < image->h; j++)
        {
            Uint32 pixel = BMP_Get_Pixel(image, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);


            if(r1 == -255)
            {
              double h, s, v;
              RGB_To_HSV(r, g, b, array);
              h = array[0];
              s = array[1];
              v = array[2];

              //printf("H: %f, S: %f, V: %f\n", h, s, v);

              // Black colors
              if(v < 0.6)
              {
                continue;
              }
              // River
              if(h > 190 && v > 0.8)
              {
                BMP_Put_Pixel(image, i, j,
                    (SDL_MapRGB(image->format, 0, 255, 0)));
                array_river[i][j] = 1;
                continue;
              }
              // Road
              if((h > 40 && v > 0.9 && s > 0.19 && h < 80) || (h == 0 && s == 0 && v == 1))
              {
                BMP_Put_Pixel(image, i, j,
                    (SDL_MapRGB(image->format, 255, 255, 255)));
                array_road[i][j] = 1;
                continue;
              }
              // White colors
              if(s < 0.17 && v > 0.87 && h > 45)
              {
                continue;
              }
              // Brown colors
              if(h > 30 && h < 49.5 && v > 0.60 && (s > 0.197 || (s > 0.151 && h > 43 && h < 46.5 && s < 0.16) || (s > 0.14 && h > 47 && s < 0.158) || (s > 0.13 && h > 53) || (s > 0.1615 && h > 37 && v > 0.60)))
              {
                BMP_Put_Pixel(image, i, j,
                    (SDL_MapRGB(image->format, 255, 0, 0)));

                array_topo[i][j] = 1;
              }
            }
            else
            {
              // Color Picker
              if(r == r1 && g == g1 && b == b1)
              {
                BMP_Put_Pixel(image, i, j,
                    (SDL_MapRGB(image->format, 255, 0, 0)));

                array_topo[i][j] = 1;
              }
            }
        }
    }

    SDL_Surface *lines = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);
    
    rebuilt_lines(lines, array_topo, bp);

    SDL_Surface *pic_river = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);

    SDL_Surface *pic_road = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);

     SDL_Surface *pic_topo = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);

    bmp_white(pic_river);
    bmp_white(pic_road);
    bmp_white(pic_topo);

    bmp_create(pic_river, array_river, "river.bmp");
    bmp_create(pic_road, array_road, "road.bmp");
    bmp_create(pic_topo, array_topo, "topo.bmp");

    // Free Surface
    free(array);
    SDL_FreeSurface(pic_river);
    SDL_FreeSurface(pic_road);
    SDL_FreeSurface(pic_topo);
    SDL_UnlockSurface(image);
    SDL_SaveBMP(image, "Pictures/Results/ign.bmp");
}

