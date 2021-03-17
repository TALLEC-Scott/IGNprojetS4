#include "MapColorisation.h"

/*void print_queue(struct queue* q) {
  printf("%i\n", q->r);
}*/

// create_queue creates and returns a queue
struct queue* create_queue()
{
  struct queue* q = malloc(sizeof(struct queue));
  q->items = calloc(0, sizeof(struct point*));
  q->f = -1;
  q->r = -1;
  return q;
}

// create_queue_int creates and returns a queue
struct queue_int* create_queue_int()
{
  struct queue_int* q = malloc(sizeof(struct queue_int));
  q->data = calloc(0, sizeof(int));
  q->f = -1;
  q->r = -1;
  return q;
}

// is_empty_int returns 1 if is empty else 0;
int is_empty_int(struct queue_int *q)
{
  if(q->r == -1)
    return 1;
  return 0;
}

// is_empty returns 1 if is empty else 0;
int is_empty(struct queue *q)
{
  if(q->r == -1)
    return 1;
  return 0;
}

// enqueue enqueues a struct point p into a struct queue q
void enqueue(struct queue* q, struct point* p, int *size)
{
  if(q->r == *size-1)
  {
    *size +=1;
    void *temp = realloc(q->items, *size * sizeof(struct point*));
    if(temp == NULL)
      printf("An error occured !\n");

    q->items = temp;
  }
  if(q->f == -1)
    q->f = 0;
  q->r += 1;
  q->items[q->r] = p;
}

// enqueue_int enqueues a struct point p into a struct queue q
void enqueue_int(struct queue_int* q, int i, int *size)
{
  if(q->r == *size-1)
  {
    *size +=1;
    void *temp = realloc(q->data, *size * sizeof(int));
    if(temp == NULL)
      printf("An error occured !\n");

    q->data = temp;
  }
  if(q->f == -1)
    q->f = 0;
  q->r += 1;
  q->data[q->r] = i;
}


// dequeue return the last element of the queue q
struct point* dequeue(struct queue* q)
{
  struct point* p;
  if(is_empty(q))
  {
    return NULL;
  }
  else
  {
    p = q->items[q->f];
    q->f++;
    if(q->f > q->r)
    {
      q->f = -1;
      q->r = -1;
    }
  }
  //print_queue(q);
  return p;
}

// dequeue_int return the last element of the queue q
int dequeue_int(struct queue_int* q)
{
  int i;
  if(is_empty_int(q))
  {
    return -1;
  }
  else
  {
    i = q->data[q->f];
    q->f++;
    if(q->f > q->r)
    {
      q->f = -1;
      q->r = -1;
    }
  }
  //print_queue(q);
  return i;
}


// Map_Colorisation : Colors topographic line in BMP image file
void Map_Colorisation(SDL_Surface *image)
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
        bfs_test(image, i, j, label, tab, res);


        //int close = map_dfs_finder(image, i, j, label, tab, 1);
        if(res[0])
        {
          printf("FIND\n");
          printf("NB : %i\n", res[1]);
          printf("Label : %i\n\n", label);
          node[size-1][0] = res[1];
          node[size-1][1] = label;

          size++;
          node = (int**)realloc(node, size *  sizeof(int*));
          node[size-1] = (int*)calloc(2, sizeof(int));
        }
        
        //clean(tab, image->w, image->h);
        label++;
      }
    }
  }


  //BMP_Test(image, tab);

  int **h = NULL;
  h = (int**)calloc(image->w, sizeof(int*));
  for(int k = 0; k < image->w; k++)
  {
    h[k] = (int*)calloc(image->h, sizeof(int));
  }

  int elevation = 1500;

  for(int i = 0; i < size; i++)
  {
    if(node[i][0] == 1)
    {
      int *next = map_elevation(image, tab, h, node[i][1], elevation);
      printf("LABEL : %i\n", node[i][1]);
      elevation -= 500;
      label = next[0];
      printf("LABEL 2 : %i\n", label);
      //next = map_elevation(image, tab, h, label, elevation);
    }
  }

  bmp_test2(image, h);

  for(int i = 0; i < image->w; i++)
  {
    free(tab[i]);
  }
  free(tab);
  for(int j = 0; j < size; j++)
  {
    free(node[j]);
  }
  free(node);
  free(res);
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, "Pictures/Results/image.bmp");
  printf("[MAP COLORISATION] Successful saved\n");
}

int* map_elevation(SDL_Surface *image, int **tab, int **h, int label,
    int elevation)
{
  
  int *list = malloc(sizeof(int));
  int size = 1;
  int next = 0;
  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      
      Uint32 pixel = BMP_Get_Pixel(image, i, j);
      Uint8 r, g, b;
      SDL_GetRGB(pixel, image->format, &r, &g, &b);

      if(tab[i][j] == label && r == 255)
      {
        h[i][j] = elevation;
      }
      else if(tab[i][j] == label)
      {
        //printf("test");
         next = dfs_elevation(image, i, j, tab, label, 0);
         //printf("Test : %i\n", next);
         list[size-1] = next;
         size++;
         list = realloc(list, size* sizeof(int));
      }
    }
  }
  clean_label(tab, image->w, image->h, label);
  return list;
}

int dfs_elevation(SDL_Surface *image, int x, int y, int **tab, int label,
    int next)
{ 
  Uint8 r, g, b; 
  Uint32 pixel = BMP_Get_Pixel(image, x, y);
  SDL_GetRGB(pixel, image->format, &r, &g, &b);
  int w = image->w;
  int h = image->h;

  if(r == 0 && tab[x][y] == label)
  {
    tab[x][y] = -1;
    if(x-1 >= 0)
    {
      next = dfs_elevation(image, x-1, y, tab, label, next);
      if(next != 0)
        return next;
    }

    if(x+1 < w)
    {
      next = dfs_elevation(image, x+1, y, tab, label, next);
      if(next != 0)
        return next;
    }

    if(y-1 >= 0)
    {
      next = dfs_elevation(image, x, y-1, tab, label, next);
      if(next != 0)
        return next;
    }

    if(y+1 < h)
    {
      next = dfs_elevation(image, x, y+1, tab, label, next);
      if(next != 0)
        return next;
    }

    if(x+1 < w && y+1 < h)
    {
      next = dfs_elevation(image, x+1, y+1, tab, label, next);
      if(next != 0)
        return next;
    }

    if(x+1 < w && y-1 >= 0)
    {
      next = dfs_elevation(image, x+1, y-1, tab, label, next);
      if(next != 0)
        return next;
    }

    if(x-1 >= 0 && y+1 < h)
    {
      next = dfs_elevation(image, x-1, y+1, tab, label, next);
      if(next != 0)
        return next;
    }

    if(x-1 >= 0 && y-1 >=0)
    {
      next = dfs_elevation(image, x-1, y-1, tab, label, next);
      if(next != 0)
        return next;
    }
  }
  else if(tab[x][y] != label && tab[x][y] != -1 && r == 255)
  {
    printf("test");
    next = tab[x][y];
    //tab[x][y] = -1;
    return next;
  }
  return next;
}

// dfs_test performs BFS into all white pixel from x / y
// [res] array stores informations about this zone and if this is a circle
// it stores the number of black lines
void bfs_test(SDL_Surface *image, int x, int y, int label,
    int **tab, int *res)
{
  int w = image->w;
  int h = image->h;
  int nb = 1;
  int count = 1;
  int* size = malloc(sizeof(int*));
  Uint8 r, g, b;
  Uint32 pixel;

  *size = 0;

  struct queue* q = create_queue();
  struct point* p = malloc(sizeof(struct point));
  p->x = x;
  p->y = y;
  enqueue(q, p, size);

  int n[4][2] = 
  {
    {0, 1},
    {0, -1},
    {-1, 0},
    {1, 0},
  };

  while(!is_empty(q))
  {
    struct point* p = dequeue(q);
    int xt = p->x;
    int yt = p->y;
    tab[xt][yt] = label;

    for(int i = 0; i < 4; i++)
    {
      if(xt+n[i][0] >= 0 && xt+n[i][0] < w && yt+n[i][1] >= 0 && yt+n[i][1] < h)
      {
        pixel = BMP_Get_Pixel(image, xt+n[i][0], yt+n[i][1]);
        SDL_GetRGB(pixel, image->format, &r, &g, &b);
        if(r == 255 && tab[xt+n[i][0]][yt+n[i][1]] == 0)
        {
          struct point* node = malloc(sizeof(struct point));
          node->x = xt+n[i][0];
          node->y = yt+n[i][1];
          tab[xt+n[i][0]][yt+n[i][1]] = label;
          enqueue(q, node, size);
        }
        else if(tab[xt+n[i][0]][yt+n[i][1]] == 0)
        {
          dfs(image, xt+n[i][0], yt+n[i][1], tab, label);
          count += 1;
        }
      }
      else
      {
        nb = 0;
      }
    }
    free(p);
  }
  free(q);
  res[0] = nb; 
  res[1] = count;
}

/*// map_dfs_finder : Performs DSF in order to find all black pixel neighborhoods
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
      nb = (nb / 10) * 10;
    }

    if(x+1 < w)
    {
      nb = map_dfs_finder(image, x+1, y, label, tab, nb);
    }
    else
    {
      nb = (nb / 10)*10;
    }

    if(y-1 >= 0)
    {
      nb = map_dfs_finder(image, x, y-1, label, tab, nb);
    }
    else
    {
      nb = (nb / 10)*10;
    }

    if(y+1 < h)
    {
       nb = map_dfs_finder(image, x, y+1, label, tab, nb);
    }
    else
    {
      nb = (nb / 10)*10;
    }
  }
  else
  {
    if(tab[x][y] == 0 && nb%10 == 1)
    {
      //dfs(image, x, y, tab, label);
      //nb += 10;
    }
  }
  return nb;
}*/

// dfs into black pixel in bmp image
void dfs(SDL_Surface *image, int x, int y, int **tab, int label)
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
    {
      dfs(image, x-1, y, tab, label);
    }

    if(x+1 < w)
    {
     dfs(image, x+1, y, tab, label);
    }

    if(y-1 >= 0)
    {
      dfs(image, x, y-1, tab, label);
    }

    if(y+1 < h)
    {
      dfs(image, x, y+1, tab, label);
    }

    if(x+1 < w && y+1 < h)
    {
      dfs(image, x+1, y+1, tab, label);
    }

    if(x+1 < w && y-1 >= 0)
    {
      dfs(image, x+1, y-1, tab, label);
    }

    if(x-1 >= 0 && y+1 < h)
    {
      dfs(image, x-1, y+1, tab, label);
    }

    if(x-1 >= 0 && y-1 >=0)
    {
      dfs(image, x-1, y-1, tab, label);
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
/*void clean(int **tab, int w, int h)
{
  for(int i = 0; i < w; i++)
  {
    for(int j = 0; j < h; j++)
    {
      if(tab[i][j] == -1)
        tab[i][j] = 0;
    }
  }
}*/

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


