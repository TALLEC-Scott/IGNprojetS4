#include "MapColorisation.h"
#include <err.h>

// Map_Colorisation : Colors topographic line in BMP image file
void Map_Colorisation(SDL_Surface *image, int **bp)
{
  SDL_LockSurface(image);
  int **tab = NULL;
  int **node = NULL;

  int size = 1;
  node = (int**)calloc(1, sizeof(int*));
  node[0] = (int*)calloc(2, sizeof(int));

  int* res = (int*) malloc(2 *  sizeof(int));
  if(res == NULL)
    printf("NUll\n");

  tab = (int**)calloc(image->w, sizeof(int*));
  for(int k = 0; k < image->w; k++)
  {
    tab[k] = (int*)calloc(image->h, sizeof(int));
  }

  int label = 1;
  
  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      Uint32 pixel = BMP_Get_Pixel(image, i, j);
      Uint8 r, g, b;
      SDL_GetRGB(pixel, image->format, &r, &g, &b);
      if(r == 255 && tab[i][j] == 0)
      {
        bfs_test(image, i, j, label, tab, res, bp);
        //int close = map_dfs_finder(image, i, j, label, tab, 1);
        if(res[0] && res[0] != -1)
        {
          //printf("FIND\n");
          //printf("NB : %i\n", res[1]);
          //printf("Label : %i\n\n", label);
          node[size-1][0] = res[1];
          node[size-1][1] = label;

          size++;
          node = (int**)realloc(node, size *  sizeof(int*));
          node[size-1] = (int*)calloc(2, sizeof(int));
        }
        label++;
      }
    }
  }
  int *M = NULL;
  M = calloc(label+1, sizeof(int));
  if(M == NULL)
  {
    errx(EXIT_FAILURE, "invalid calloc M");
  }
  printf("Label %i\n", label);

  //BMP_Test(image, tab);
  int **h = NULL;
  h = (int**)calloc(image->w, sizeof(int*));
  if(h == NULL)
  {
    errx(EXIT_FAILURE, "invalid calloc h");
  }
  for(int k = 0; k < image->w; k++)
  {
    h[k] = (int*)calloc(image->h, sizeof(int));
  }

  int elevation = 1500;
  int size_q = 0;
  struct queue_int* q = NULL;

  for(int i = 0; i < size; i++)
  {
    if(node[i][0] == 1)
    {
      if(M[node[i][1]] == 0)
      {
        printf("label start %i\n", node[i][1]);
        q = enqueue_int(q, node[i][1]);
        size_q +=1;
        M[node[i][1]] = 1;
      }
              /*int *next = map_elevation(image, tab, h, node[i][1], elevation);
      printf("LABEL : %i\n", node[i][1]);
      elevation -= 500;
      label = next[0];
      printf("LABEL 2 : %i\n", label);
      next = map_elevation(image, tab, h, label, elevation);*/
    }
  }
  int counter = size_q;
  int counter_temp = 0;

  int *size_e = malloc(sizeof(int));
  if(size_e == NULL)
  {
    errx(EXIT_FAILURE, "invalid malloc size_e");
  }
  *size_e = 1;
  int label_temp = 0;

  while(!is_empty_int(q))
  {
    if(counter == 0)
    {
      elevation -= 100;
      if(elevation == 0)
        elevation = -100;
      counter = counter_temp;
      counter_temp = 0;
    }

    q = dequeue_int(q, &label_temp);
    size_q -= 1;
    *size_e = 1;
    int *next = map_elevation(image, tab, h, label_temp, elevation, size_e);
    //printf("Elevation %i\n", elevation);
    //printf("Label : %i\n", label);

    if(next == NULL || *size_e == 1)
    {
      counter--;
      free(next);
      continue;
    }
    //printf("Len : %i\n", *size_e -1);
    for(int i = 0; i < *size_e; i++)
    {
      if(next[i] > 0 && next[i] <= label && M[next[i]] == 0)
      {
        q = enqueue_int(q, next[i]);
        size_q += 1;
        //printf("label %i\n", next[i]);
        M[next[i]] = 1;
        //printf("next : %i\n", next[i]);
        counter_temp++;
      }
      
    }
    free(next);
    next = NULL;
    counter--;
  }
  bmp_test3(image, bp, h);
  
  // BP matrix of elevation of blakc pixel
  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      if(bp[i][j] == 1)
      {
        bp[i][j] = h[i][j];
      }
    }
  }

  

  int **tab2 = NULL;
  tab2 = (int**)calloc(image->w, sizeof(int*));
  for(int k = 0; k < image->w; k++)
  {
    tab2[k] = (int*)calloc(image->h, sizeof(int));
  }

  map_set_altitude(h, tab2, tab, 0, 400, 1500, image->w, image->h, 1);
  bmp_test2(image, h);


  /* FREE */

  free(size_e);
  free(M);

  for(int i = 0; i < image->w; i++)
  {
    free(tab[i]);
    free(tab2[i]);
    free(h[i]);
  }
  free(tab2);
  free(tab);
  free(h);

  for(int j = 0; j < size; j++)
  {
    free(node[j]);
  }
  free(node);

  free(res);

  /* END FREE */
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, "Pictures/Results/image.bmp");
  printf("[MAP COLORISATION] Successful saved\n");


}

// map_remplace_label replaces all old label by new label
void map_remplace_label(int **h2, int **tab, int w, int h, int label_old, int new)
{
  for(int i = 0; i < w; i++)
  {
    for(int j = 0; j < h; j++)
    {
      if(tab[i][j] == label_old)
      {
        h2[i][j] = new;
      }
    }
  }
}
void map_set_altitude(int **h2, int **tab, int **tab2, int x, int y, int new,
    int w, int h, int manual)
{
  int label_old = tab2[x][y];
  map_remplace_label(h2, tab2, w, h, label_old, new);

  if(!manual)
  {
    struct queue* q = NULL;
    struct point p;
    p.x = label_old;
    p.y = -1;
    q = enqueue(q, p);

    while(!is_empty(q))
    {
      struct point p;
      q = dequeue(q, &p);
      int label = p.x;

      struct point *list = calloc(10, sizeof(struct point));
      int size = 1;

      int find = 0;
      for(int i = 0; i < w && !find; i++)
      {
        for(int j = 0; j < h && !find; j++)
        {
          if(tab2[i][j] == label)
          {
            find = 1;
            bfs_set_altitude(i, j, w, h, tab, tab2, h2, h2[i][j], label, list, &size);
          }
        }
      }

      for(int i = 0; i < size-1; i++)
      {
        q = enqueue(q, list[i]);
        printf("Enqueu : %i\n", list[i].x);
      }
      free(list);
    }
  }
}

void bfs_set_altitude(int x, int y, int w, int h, int **tab, int **tab2, int **h2,
    int label, int label_old, struct point *list, int *size)
{
  struct queue* q = NULL;
  struct point p;
  p.x = x;
  p.y = y;
  q = enqueue(q, p);
  int total = 10;

  int n[4][2] = 
  {
    {0, 1},
    {0, -1},
    {-1, 0},
    {1, 0},
  };

  while(!is_empty(q))
  {
    struct point p;
    q = dequeue(q, &p);
    int xt = p.x;
    int yt = p.y;
    

    tab[xt][yt] = 1;

    for(int i = 0; i < 4; i++)
    {
      if(xt+n[i][0] >= 0 && xt+n[i][0] < w && yt+n[i][1] >= 0 && yt+n[i][1] < h)
      {
        int x1 = xt+n[i][0];
        int y1 = yt+n[i][1];

        if(h2[x1][y1] == label && tab[x1][y1] == 0 && tab2[x1][y1] == label_old)
        {
          
          struct point node;
          node.x = x1;
          node.y = y1;
          tab[x1][y1] = 1;
          q = enqueue(q, node);
        }
        else if(tab2[x1][y1] != label_old && tab[x1][y1] == 0)
        {
          if(h2[x1][y1] - label >= 100 || label-h2[x1][y1] >= 100)
          {
            int new_label = h2[x1][y1];
            if(new_label > label)
            {
              while(new_label-100  > label)
              {
                new_label -= 100;
              }
            }
            else
            {
              while(new_label+100 < label)
              {
                new_label += 100;
              }
            }
            
            // Mark all pixels
            for(int i = 0; i < w; i++)
            {
              for(int j = 0; j < h; j++)
              {
                if(tab2[i][j] == tab2[x1][y1])
                {
                  tab[i][j] = 1;
                }
              }
            } 

            // Check if label is already saves
            if(!is_present2(list, *size, tab2[x1][y1]))
            {
              struct point new_p;
              new_p.x = tab2[x1][y1];
              new_p.y = label;
              list[*size-1] = new_p;
              *size+=1;
              if(*size >= total)
              {
                total = total * 2;
                list = realloc(list, total * sizeof(struct point));
                if(list == NULL)
                {
                  errx(EXIT_FAILURE, "Invalid realloc list");
                }
              }
            }

            // Replace all previous altitude by the new one
            map_remplace_label(h2, tab2, w, h, tab2[x1][y1], new_label);
          }
        }
      }
    }
  }
}

int* map_elevation(SDL_Surface *image, int **tab, int **h, int label,
    int elevation, int* size)
{
  
  int *list = calloc(10, (sizeof(int)));
  if(list == NULL)
  {
    errx(EXIT_FAILURE, "Invalid calloc list");
  }
  int total = 10;

  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      Uint32 pixel = BMP_Get_Pixel(image, i, j);
      Uint8 r, g, b;
      SDL_GetRGB(pixel, image->format, &r, &g, &b);

      if(tab[i][j] == label && r == 255 && h[i][j] == 0)
      {
        bfs_elevation(image, i, j, label, tab, h, list, size, elevation, total);
        //map_elevation_colorize(h, tab, label, elevation, image->w, image->h);
      }
    }
  }
  clean_label(tab, image->w, image->h, label);
  if(list[0] == 0)
  {
    return NULL;
    free(list);
  }
  return list;
}

// map_elevation_colorize
void map_elevation_colorize(int **h, int **tab, int label, int elevation,
    int w, int h2)
{
  for(int i = 0; i < w; i++)
  {
    for(int j = 0; j < h2; j++)
    {
      if((tab[i][j] == label || tab[i][j] == -1) && h[i][j] != elevation)
      {
        h[i][j] = elevation;
      }
    }
  }
}

// bfs_elevation -> TEST
void bfs_elevation(SDL_Surface *image, int x, int y, int label,
    int **tab, int **h2, int* list, int* size_2, int elevation, int total)
{
  int w = image->w;
  int h = image->h;

  Uint8 r, g, b;
  Uint32 pixel;

  struct queue* q = NULL;
  struct point p;
  p.x = x;
  p.y = y;
  q = enqueue(q, p);

  int n[4][2] = 
  {
    {0, 1},
    {0, -1},
    {-1, 0},
    {1, 0},
  };
  printf("TEST\n");
  printf("Elevation : %i\n", elevation);

  while(!is_empty(q))
  {
    struct point p;
    q = dequeue(q, &p);
    int xt = p.x;
    int yt = p.y;
    //tab[xt][yt] = -1;
    //
    h2[xt][yt] = elevation;

    for(int i = 0; i < 4; i++)
    {
      if(xt+n[i][0] >= 0 && xt+n[i][0] < w && yt+n[i][1] >= 0 && yt+n[i][1] < h)
      {

        pixel = BMP_Get_Pixel(image, xt+n[i][0], yt+n[i][1]);
        SDL_GetRGB(pixel, image->format, &r, &g, &b);

        if(tab[xt+n[i][0]][yt+n[i][1]] == label &&
            h2[xt+n[i][0]][yt+n[i][1]] == 0 &&r == 255)
        {
          struct point node;
          node.x = xt+n[i][0];
          node.y = yt+n[i][1];
          //tab[xt+n[i][0]][yt+n[i][1]] = -1;

          h2[xt+n[i][0]][yt+n[i][1]] = elevation;
          q = enqueue(q, node);
        }
        else if(/*tab[xt+n[i][0]][yt+n[i][1]] != -1*/ r == 0 &&
            h2[xt+n[i][0]][yt+n[i][1]] == 0)
        {
          int new_label = 0;
          dfs_elevation(image, xt+n[i][0], yt+n[i][1], tab,
              tab[xt+n[i][0]][yt+n[i][1]], h2, elevation, &new_label);
          //printf("Label : %i\n", new_label);

          if(!is_present(list, *size_2, tab[xt+n[i][0]][yt+n[i][1]]) && tab[xt+n[i][0]][yt+n[i][1]] != label)
          {
            //printf("Label : %i\n", tab[xt+n[i][0]][yt+n[i][1]]);
            list[*size_2-1] = tab[xt+n[i][0]][yt+n[i][1]];
            *size_2 += 1;
            if(*size_2 == total)
            {
              total = total *2;
              list = realloc(list, total * sizeof(int));
              if(list == NULL)
              {
                errx(EXIT_FAILURE, "invalid realloc list");
              }
            }
          }

          if(new_label != 0 && !is_present(list, *size_2, new_label))
          {
            //printf("Label : %i\n", tab[xt+n[i][0]][yt+n[i][1]]);
            list[*size_2-1] = new_label;
            *size_2 += 1;
            if(*size_2 == total)
            {
              total = total * 2;
              list = realloc(list, total * sizeof(int));
              if(list == NULL)
              {
                errx(EXIT_FAILURE, "invalid realloc list");
              }
            }
          }
        }
      }
    }
  } 
}

int is_present(int* list, int size, int x)
{
  for(int i = 0; i <= size-2; i++)
  {
    if(list[i] == x)
      return 1;
  }
  return 0;
}

int is_present2(struct point *list, int size, int x)
{
  for(int i = 0; i <= size-2; i++)
  {
    struct point p = list[i];
    if(p.x == x)
    {
      return 1;
    }
  }
  return 0;
}


void dfs_elevation(SDL_Surface *image, int x, int y, int **tab, int label,
    int **h, int elevation, int *new_label)
{ 
  Uint8 r, g, b; 
  Uint32 pixel = BMP_Get_Pixel(image, x, y);
  SDL_GetRGB(pixel, image->format, &r, &g, &b);
  int w = image->w;
  int h2 = image->h;

  if(r == 0 && h[x][y] == 0 && tab[x][y] == label)
  {
    h[x][y] = elevation;

    if(x-1 >= 0)
    {
      dfs_elevation(image, x-1, y, tab, label, h, elevation, new_label);
    }

    if(x+1 < w)
    {
      dfs_elevation(image, x+1, y, tab, label, h, elevation, new_label);
    }

    if(y-1 >= 0)
    {
      dfs_elevation(image, x, y-1, tab, label, h, elevation, new_label);
    }

    if(y+1 < h2)
    {
      dfs_elevation(image, x, y+1, tab, label, h, elevation, new_label);
    }

    if(x+1 < w && y+1 < h2)
    {
      dfs_elevation(image, x+1, y+1, tab, label, h, elevation, new_label);
    }

    if(x+1 < w && y-1 >= 0)
    {
      dfs_elevation(image, x+1, y-1, tab, label, h, elevation, new_label);
    }

    if(x-1 >= 0 && y+1 < h2)
    {
      dfs_elevation(image, x-1, y+1, tab, label, h, elevation, new_label);
    }

    if(x-1 >= 0 && y-1 >=0)
    {
      dfs_elevation(image, x-1, y-1, tab, label, h, elevation, new_label);
    }
  }
  else if(r == 255 && tab[x][y] != label)
  {
    *new_label = tab[x][y];
  }
}

// dfs_test performs BFS into all white pixel from x / y
// [res] array stores informations about this zone and if this is a circle
// it stores the number of black lines
void bfs_test(SDL_Surface *image, int x, int y, int label,
    int **tab, int *res, int **bp)
{
  int w = image->w;
  int h = image->h;
  int nb = 1;
  int count = 1;
  Uint8 r, g, b;
  Uint32 pixel;
  int nb_pixel = 0;
  int label_nb_pixel = -1;

  struct queue* q = NULL;
  struct point p;
  p.x = x;
  p.y = y;
  q = enqueue(q, p);

  int n[4][2] = 
  {
    {0, 1},
    {0, -1},
    {-1, 0},
    {1, 0},
  };

  while(!is_empty(q))
  {
    struct point p;
    q = dequeue(q, &p);
    int xt = p.x;
    int yt = p.y;
    tab[xt][yt] = label;

    for(int i = 0; i < 4; i++)
    {
      if(xt+n[i][0] >= 0 && xt+n[i][0] < w && yt+n[i][1] >= 0 && yt+n[i][1] < h)
      {
        pixel = BMP_Get_Pixel(image, xt+n[i][0], yt+n[i][1]);
        SDL_GetRGB(pixel, image->format, &r, &g, &b);
        if(r == 255 && tab[xt+n[i][0]][yt+n[i][1]] == 0)
        {
          struct point node;
          node.x = xt+n[i][0];
          node.y = yt+n[i][1];
          tab[xt+n[i][0]][yt+n[i][1]] = label;
          q = enqueue(q, node);
          nb_pixel += 1;
        }
        else if(tab[xt+n[i][0]][yt+n[i][1]] == 0)
        {
          dfs(image, xt+n[i][0], yt+n[i][1], tab, label, bp);
          count += 1;
        }
        else
        {
          label_nb_pixel = tab[xt+n[i][0]][yt+n[i][1]];
        }
      }
      else
      {
        nb = 0;
      }
    }
  }
  if(nb_pixel <= 55)
  {
    remove_label(image, tab, w, h, label, bp, label_nb_pixel);
    res[0] = -1;
    res[1] = -1;
  }
  else
  {
    res[0] = nb; 
    res[1] = count;
  }
}

// dfs into black pixel in bmp image
void dfs(SDL_Surface *image, int x, int y, int **tab, int label, int **bp)
{
  Uint8 r, g, b; 
  Uint32 pixel = BMP_Get_Pixel(image, x, y);
  SDL_GetRGB(pixel, image->format, &r, &g, &b);
  int w = image->w;
  int h = image->h;

  if(r == 0 && tab[x][y] == 0)
  {
    tab[x][y] = label;
    bp[x][y] = 1;

    if(x-1 >= 0)
    {
      dfs(image, x-1, y, tab, label, bp);
    }

    if(x+1 < w)
    {
     dfs(image, x+1, y, tab, label, bp);
    }

    if(y-1 >= 0)
    {
      dfs(image, x, y-1, tab, label, bp);
    }

    if(y+1 < h)
    {
      dfs(image, x, y+1, tab, label, bp);
    }

    if(x+1 < w && y+1 < h)
    {
      dfs(image, x+1, y+1, tab, label, bp);
    }

    if(x+1 < w && y-1 >= 0)
    {
      dfs(image, x+1, y-1, tab, label, bp);
    }

    if(x-1 >= 0 && y+1 < h)
    {
      dfs(image, x-1, y+1, tab, label, bp);
    }

    if(x-1 >= 0 && y-1 >=0)
    {
      dfs(image, x-1, y-1, tab, label, bp);
    }
  }
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

// clean tab array remove all -1
void clean_label(int **tab, int w, int h, int label)
{
  for(int i = 0; i < w; i++)
  {
    for(int j = 0; j < h; j++)
    {
      if(tab[i][j] == -1)
        tab[i][j] = label;
    }
  }
}

// Replace all pixel with label "label" with black pixel and reset her label
void remove_label(SDL_Surface *image, int **tab, int w, int h, int label,
    int **bp, int label2)
{
  for(int i = 0; i < w; i++)
  {
    for(int j = 0; j < h; j++)
    {
      if(tab[i][j] == label)
      {
        bp[i][j] = 1;
        if(label2 == -1)
        {
          tab[i][j] = 0;
        }
        else
        {
          tab[i][j] = label2;
        }
        BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 0)));
      }
    }
  }
}



