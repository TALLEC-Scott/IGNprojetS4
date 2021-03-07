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
  /*int tab[image->h][image->h];
  for(int k = 0; k < image->w; k++)
  {
    for(int l = 0; l < image->h; l++)
    {
      tab[k][l] = 0;
    }
  }*/

  int label = 1;

  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      Uint32 pixel = BMP_Get_Pixel(image, i, j);
      Uint8 r, g, b;
      SDL_GetRGB(pixel, image->format, &r, &g, &b);
      if(r == 0 && tab[i][j] == 0)
      {
        Map_DFS_Fill(image, i, j, label, tab);
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
}

// Map_DFS_Fill : Performs DSF in order to find all black pixel neighborhoods
// and labels them
void Map_DFS_Fill(SDL_Surface *image, int x, int y, int label,
int **tab)
{
  Uint8 r, g, b; 
  Uint32 pixel = BMP_Get_Pixel(image, x, y);
  SDL_GetRGB(pixel, image->format, &r, &g, &b);
  int w = image->w;
  int h = image->h;

  if(r == 0 && tab[x][y] == 0)
  {
    tab[x][y] = label;
    if(x-1 >= 0)
      Map_DFS_Fill(image, x-1, y, label, tab);

    if(x+1 < w)
      Map_DFS_Fill(image, x+1, y, label, tab);

    if(x-1 >= 0 && y+1 < h)
      Map_DFS_Fill(image, x-1, y+1, label, tab);

    if(x+1 < w && y+1 < h)
      Map_DFS_Fill(image, x+1, y+1, label, tab);

    if(x-1 >= 0 && y-1 >= 0)
      Map_DFS_Fill(image, x-1, y-1, label, tab);

    if(x+1 < w && y-1 >= 0)
      Map_DFS_Fill(image, x+1, y-1, label, tab);

    if(y-1 >= 0)
      Map_DFS_Fill(image, x, y-1, label, tab);

    if(y+1 < h)
      Map_DFS_Fill(image, x, y+1, label, tab);
  }
}


