#include "MapColorisation.h"

// Map_Colorisation : Colors topographic line in BMP image file
void Map_Colorisation(SDL_Surface *image)
{
  SDL_LockSurface(image);
  int **tab = NULL;
  tab = (int**)calloc(image->w, sizeof(int*));
  for(int k = 0; k < image->w; k++)
  {
    tab[k] = (int*)calloc(image->h, sizeof(int*));
  }

  int label = 1;
  int close = 0;

  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      Uint32 pixel = BMP_Get_Pixel(image, i, j);
      Uint8 r, g, b;
      SDL_GetRGB(pixel, image->format, &r, &g, &b);
      if(r == 255 && tab[i][j] == 0)
      {
        close = map_dfs_finder(image, i, j, label, tab, 1);
        if(close)
          printf("FIND\n");
        label++;
      }
    }
  }


  BMP_Test(image, tab);
  for(int i = 0; i < image->w; i++)
  {
    free(tab[i]);
  }
  free(tab);
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, "Pictures/Results/image.bmp");
  printf("[MAP COLORISATION] Successful saved\n");
}

// map_dfs_finder : Performs DSF in order to find all black pixel neighborhoods
// and return if this is a boucle or not
int map_dfs_finder(SDL_Surface *image, int x, int y, int label,
int **tab, int first)
{
  Uint8 r, g, b; 
  Uint32 pixel = BMP_Get_Pixel(image, x, y);
  SDL_GetRGB(pixel, image->format, &r, &g, &b);
  int w = image->w;
  int h = image->h;
  int nb = first;

  if(r == 255 && tab[x][y] == 0)
  {
    tab[x][y] = label;

    if(x-1 >= 0)
    {
      nb = map_dfs_finder(image, x-1, y, label, tab, nb);
    }
    else
    {
      nb = 0; 
    }

    if(x+1 < w)
    {
      nb = map_dfs_finder(image, x+1, y, label, tab, nb);
    }
    else
    {
      nb = 0;
    }

    if(y-1 >= 0)
    {
      nb = map_dfs_finder(image, x, y-1, label, tab, nb);
    }
    else
    {
      nb = 0; 
    }

    if(y+1 < h)
    {
      nb = map_dfs_finder(image, x, y+1, label, tab, nb);
    }
    else
    {
      nb = 0;  
    }
  }
  return nb;
}

// map_end returns 1 if the first pixel DFS is find, else 0
int map_end(int **tab, int x, int y, int h, int w)
{
  if(x-1 >= 0 && tab[x-1][y] == -1)
    return 1;
  if(x+1 < w  && tab[x+1][y] == -1)
    return 1;
  if(x-1 >= 0 && y+1 < h && tab[x-1][y+1] == -1)
    return 1;
  if(x+1 < w && y+1 < h && tab[x+1][y+1] == -1)
    return 1;
  if(x-1 >= 0 && y-1 >= 0 && tab[x-1][y-1] == -1)
    return 1;
  if(x+1 < w && y-1 >= 0 && tab[x+1][y-1] == -1)
    return 1;
  if(y-1 >= 0 && tab[x][y-1] == -1)
    return 1;
  if(y+1 < h && tab[x][y+1] == -1)
    return 1;
  return 0;
}

// is_black_pixel returns 1 if it is else 0
int is_black_pixel(SDL_Surface *image, int x, int y)
{
  Uint8 r, g, b; 
  Uint32 pixel = BMP_Get_Pixel(image, x, y);
  SDL_GetRGB(pixel, image->format, &r, &g, &b);
  return r == 0;
}


