#include "tools.h"
#include "MapColorisation.h"
#include <string.h>
#include <stdlib.h>

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

// bmp_create Puts pixel of array in image and saves it as BMP
void bmp_create(SDL_Surface *image, int **array, char *name)
{
  SDL_LockSurface(image);
  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      if(array[i][j] == 0)
        BMP_Put_Pixel(image, i, j,
                  (SDL_MapRGB(image->format, 255, 255, 255)));
      if(array[i][j] == 1)
        BMP_Put_Pixel(image, i, j,
                  (SDL_MapRGB(image->format, 0, 0, 0)));
      if(array[i][j] == 2)
        BMP_Put_Pixel(image, i, j,
                  (SDL_MapRGB(image->format, 255, 0, 0)));
    }
  }

  char res[30] = "Pictures/Results/";
  strcat(res, name);
  printf("%s [DONE]\n", res);
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, res);
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
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 128, 0, 0)));
      }
      else if(label == 1400)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 0, 0)));
      }
      else if(label == 1300)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 102, 0)));
      }
      else if(label == 1200)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 127, 0)));
      }
      else if(label == 1100)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 153, 51)));
      }
      else if(label == 1000)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 204, 102)));
      }
      else if(label == 900)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 255, 255, 153)));
      }
      else if(label == 800)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 204, 255, 153)));
      }
      else if(label == 700)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 153, 255, 153)));
      }
      else if(label == 600)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 102, 255, 153)));
      }
      else if(label == 500)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 255, 153)));
      }
      else if(label == 400)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 255, 255)));
      }
      else if(label == 300)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 204, 255)));
      }
      else if(label == 200)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 102, 204)));
      }
      else if(label == 100)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 51, 204)));
      }
      else if(label == 0)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 153)));
      }
      else if(label == -100)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 102)));
      }
      else if(label == -200)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 77)));
      }
      else if(label == -300)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 26)));
      }
      else if(label == -400)
      {
         BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format, 0, 0, 0)));
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
            //printf("temp : %f\n", temp%(double)6);
            if(temp < (double) 0)
            {
              h = (double)60 * ((double)6 - temp);
            }
            else
            {
              h = (double)60 * fmod(temp, (double)6);
            }
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

//test if a pixel is black
int is_black(SDL_Surface *image, int x, int y)
{
  Uint32 pixel = BMP_Get_Pixel(image, x, y);
  Uint8 r, g, b;
  SDL_GetRGB(pixel, image->format, &r, &g, &b);
  if (r==255 && g==255 && b==255)
	return 0;
  return 1;
}

size_t len_array_int(int *array)
{
  size_t len = 0;
  for(len = 0; array[len] != '0'; len++)
  {
  }
  return len;
}

// is_empty_int returns 1 if is empty else 0;
void empty_int(struct queue_int** pstart)
{
  while(*pstart != NULL)
  {
    *pstart = dequeue_int(*pstart, NULL);
  }
}

// is_empty returns 1 if is empty else 0;
void empty(struct queue** pstart)
{
  while(*pstart != NULL)
  {
    *pstart = dequeue(*pstart, NULL);
  }
}

// enqueue enqueues a struct point p into a struct queue q
struct queue* enqueue(struct queue* start, struct point val)
{
    struct queue *q = malloc(sizeof(struct queue));
    if(q == NULL)
    {
      errx(EXIT_FAILURE, "could not malloc the queue");
    }
    q->val = val;

    if(start == NULL)
    {
      q->next = q;
    }
    else
    {
      q->next = start->next;
      start->next = q;
    }
    return q;
}

// enqueue_int enqueues a struct point p into a struct queue q
struct queue_int* enqueue_int(struct queue_int* start, int val)
{
    struct queue_int *q = malloc(sizeof(struct queue_int));
    if(q == NULL)
    {
      errx(EXIT_FAILURE, "could not malloc the queue");
    }
    q->val = val;

    if(start == NULL)
    {
      q->next = q;
    }
    else
    {
      q->next = start->next;
      start->next = q;
    }
    return q;
}

// dequeue return the last element of the queue q
struct queue* dequeue(struct queue* start, struct point *pval)
{
  if(start == NULL)
    return NULL;
  struct queue *q = start->next;
  if(start->next == start)
  {
    *pval = q->val;
    free(start);
    return NULL;
  }
  *pval = q->val;
  start->next = q->next;
  free(q);
  q = NULL;
  return start;
}

// dequeue_int return the last element of the queue q
struct queue_int* dequeue_int(struct queue_int* start, int *pval)
{
  if(start == NULL)
    return NULL;
  struct queue_int *q = start->next;
  if(start->next == start)
  {
    *pval = q->val;
    free(start);
    return NULL;
  }
  *pval = q->val;
  start->next = q->next;
  free(q);
  q = NULL;
  return start;
}

// is_empty returns 1 if queue is empty, 0 otherwise
int is_empty(struct queue* start)
{
  return start == NULL;
}

// is_empty returns 1 if queue is empty, 0 otherwise
int is_empty_int(struct queue_int* start)
{
  return start == NULL;
}



void bmp_test3(SDL_Surface *image, int **tab, int **h)
{
  SDL_LockSurface(image);
  for(int i = 0; i < image->w; i++)
  {
    for(int j = 0; j < image->h; j++)
    {
      int label = 0;
      if(tab[i][j] == 1)
      {
        tab[i][j] = h[i][j];
        label = tab[i][j];
      
      if(label == 1500)
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
  }
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, "Pictures/Results/elevation_matrice.bmp");
}

int get_2D_array_x_y (_2D_array *M, size_t x, size_t y){

	return *(M->data)[y * M->cols + x];
}

void set_2D_array_x_y (_2D_array *M, size_t x, size_t y, size_t new_val){

	*M->data[y * M->cols + x] = new_val;
}

void load_image(Image *image, char *file)
{
    GError *error = NULL;
    GdkPixbuf *pix = NULL;

    // Save the pixbuf in image
    pix = gdk_pixbuf_new_from_file(file, &error);
    if (!error)
    {
        image->pixbuf = pix;
        image->format = (gdk_pixbuf_get_has_alpha (
                    image->pixbuf)) ? CAIRO_FORMAT_ARGB32 :
                    CAIRO_FORMAT_RGB24;
        image->width = gdk_pixbuf_get_width(image->pixbuf);
        image->height = gdk_pixbuf_get_height(image->pixbuf);

        // Resize the drawing area to trigger scrollbars
        gtk_widget_set_size_request(GTK_WIDGET(image->area),
                image->width, image->height);

        // Creates cairo surface for zoom and rotation
        image->image_surface = cairo_image_surface_create(image->format,
                image->width, image->height);

        cairo_t *cr = cairo_create(image->image_surface);

        // Links the surface to the pixbuf
        gdk_cairo_set_source_pixbuf(cr, image->pixbuf, 0, 0);

        // Draws the surface
        cairo_paint(cr);

        gtk_widget_queue_draw_area(GTK_WIDGET(image->area), 0, 0,
                image->width, image->height);

        free(cr);
    }
    else
        g_critical("%s",error->message);
}

// Takes array from extraction (river, trail, ect.) and update value with
// real altitude from h2 array
void process_array(int **array, int **h2, int h, int w)
{
  for(int i = 0; i < w; i++)
  {
    for(int j = 0; j < h; j++)
    {
      if(array[i][j] != 0 && array[i][j] != -1)
      {
        array[i][j] = h2[i][j];
      }
      else
      {
        array[i][j] = -1;
      }
    }
  }
}

