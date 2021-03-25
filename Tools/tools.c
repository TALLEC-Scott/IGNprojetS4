#include "tools.h"
#include "MapColorisation.h"
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
      if(label == -1)
      {
        BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 0)));

      }
      else if(label != 0)
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

void bmp_test2(SDL_Surface *image, int **tab)
{
  SDL_LockSurface(image);
  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      int label = tab[i][j];
      if(label == -1)
      {
        BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 0)));

      }
      else if(label == 1500)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 0, 0)));
      }
      else if(label == 1400)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 56, 0)));
      }
      else if(label == 1300)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 127, 0)));
      }
      else if(label == 1200)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 180, 0)));
      }
      else if(label == 1100)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 213, 0)));
      }
      else if(label == 1000)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 255, 0)));
      }




    }
  }
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, "Pictures/Results/elevation.bmp");

}


double Max(double r, double g, double b)
{
    if(r >= g && r >= b)
        return r;
    if(b >= r && b >= g)
        return b;
    if(g >= r && g >= b)
        return g;
    return (double)0;
}

double Min(double r, double g, double  b)
{
    if(r <= g && r <= b)
        return r;
    if(b <= r && b <= g)
        return b;
    if(g <= r && g <= b)
        return g;
    return (double)0;
}

// RGB_To_HSV converts r, g, b value to HSV format
void RGB_To_HSV(double r, double g, double b, double array[3])
{
    double r1, g1, b1, max, min, delta, h, s, v;

    r1 = r / (double)255;
    g1 = g / (double)255;
    b1 = b / (double)255;

    //printf("R1: %f, G1: %f, B1: %f\n", r1, g1, b1);

    max = Max(r1, g1, b1);
    min = Min(r1, g1, b1);
    delta = max - min;

    //printf("MAX %f, MIN %f, DELTA %f\n", max, min, delta);

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
        if(max == r1)
        {
            double temp = ((g1 - b1) / delta);
            h = (double)60 * fmod(temp, (double)6);
        }
        if(max == g1)
            h = (double)60 * (((b1 - r1) / delta) + 2);
        if(max == b1)
            h = (double)60 * (((r1 - g1) / delta) + 4);
    }
    array[0] = h;
    array[1] = s;
    array[2] = v;
}

// bmp_white Puts white pixel everywhere in SDL_Surface *image
void bmp_white(SDL_Surface *image)
{
  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 255, 255)));
    }
  }
}

size_t len_array_int(int *array)
{
  size_t len = 0;
  for(len = 0; array[len] != '0'; len++)
  {
  }
  return len;
}

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




