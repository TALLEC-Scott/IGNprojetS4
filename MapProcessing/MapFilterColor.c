#include "MapFilterColor.h"
#include "MapTexture.h"

// bmp_filter initializes array for stores all the informations about
// the ign map
void bmp_filter(SDL_Surface *image, int r, int g, int b,
    int r1, int g1, int b1, int r2, int g2, int b2
    ,int **bp, int **tab, int **h, int **road_major, int **road_minor,
    int **river, int **trail, int **name)
{

  int **topo = NULL;
  topo = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  {
    topo[i] = (int*)calloc(image->h, sizeof(int*));
  }

  /*int **river = NULL;
  river = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  {
    river[i] = (int*)calloc(image->h, sizeof(int*));
  }

  int **road_major = NULL;
  road_major = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  {
    road_major[i] = (int*)calloc(image->h, sizeof(int*));
  }

  int **road_minor = NULL;
  road_minor = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  {
    road_minor[i] = (int*)calloc(image->h, sizeof(int*));
  }


  int **trail = NULL;
  trail = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  {
    trail[i] = (int*)calloc(image->h, sizeof(int*));
  }*/


  filter(image, topo, river, road_major, road_minor, trail, r, g, b, r1, g1,
      b1, r2, g2, b2, bp, tab, h, name);

  // free
  for(int i = 0; i < image->w; i++)
  {
    free(topo[i]);
    /*free(river[i]);
    free(road_major[i]);
    free(road_minor[i]);
    free(trail[i]);*/
  }

  free(topo);
  /*free(river);
  free(road_major);
  free(road_minor);
  free(trail);*/
}

void rec_sampling(int ***arr, int w, int h, int **mark, int ngh[8][2], int count, int x, int y)
{
  int** array = *arr;
  mark[x][y] = 1;
  for (int i = 0; i<8; i++)
  {
  	int nw_x = x + ngh[i][0];
  	int nw_y = y + ngh[i][1];
	if (nw_x>=0 && nw_x<w && nw_y>=0 && nw_y<h 
		&& mark[nw_x][nw_y] == 0 && array[nw_x][nw_y] != -1)
	{
		if (count == 5)
		{
			count = -1;
		}
		else
		{
			array[x][y] = -1;
		}
		rec_sampling(arr, w, h, mark, ngh, count+1, nw_x, nw_y);
	}
  }
}

//line point sampling 
void sampling(int ***arr, int w, int h)
{
  int** array = *arr;
  int **mark = NULL;
  mark = (int**)calloc(w, sizeof(int*));
  for(int i = 0; i < w; i++)
	mark[i] = (int*)calloc(h, sizeof(int*));
  int fd_ngh[8][2] = {{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0}};
  for (int i = 0; i<w; i++)
  {
  	for (int j = 0; j<h; j++)
  	{
  		if (mark[i][j] == 0 && array[i][j] != -1)
  			rec_sampling(arr, w, h, mark, fd_ngh, 0, i, j);
  	}
  }
  for (int i=0; i<w; i++)
	free(mark[i]);
  free(mark);
}

// filter Converts all rgb values to HSV in order to filters colors
void filter(SDL_Surface *image, int **array_topo, int **array_river,
    int **array_road_major, int **array_road_minor, int **array_trail, int r1,
    int g1, int b1, int r2, int g2, int b2, int r3, int g3, int b3, 
    int **bp, int **tab, int **h, int **array_name)
{
    double *array = calloc(3, sizeof(double));
    SDL_LockSurface(image);

    printf("r %i, g, %i, b, %i", r1, g1, b1);

    printf("r %d\n", r1);
    for(int i = 0; i < image->w; i++)
    {
        for(int j = 0; j < image->h; j++)
        {
            Uint32 pixel = BMP_Get_Pixel(image, i, j);
            Uint8 r, g, b;
            SDL_GetRGB(pixel, image->format, &r, &g, &b);


            if(r1 == 0)
            {
              double h, s, v;
              RGB_To_HSV(r, g, b, array);
              h = array[0];
              s = array[1];
              v = array[2];

              //printf("H: %f, S: %f, V: %f\n", h, s, v);

              //Name colors
              if(h < 17.2 && s > 0.19 && v > 0.5 && v < 0.73 && s < 0.64)
              {
                array_name[i][j] = 1;
                continue;
              }
              
              // Trail colors
              if(h > 350 && s > 0.18)
              {
                array_trail[i][j] = 1;
                continue;
              }

              // Black colors
              if(v < 0.6)
              {
                continue;
              }
              // River
              if(h > 190 && v > 0.8 && h < 350 && s > 0.1)
              {
                BMP_Put_Pixel(image, i, j,
                    (SDL_MapRGB(image->format, 0, 255, 0)));
                array_river[i][j] = 1;
                continue;
              }
              // Road major
              if(h > 40 && v > 0.9 && s > 0.19 && h < 80)
              {
                BMP_Put_Pixel(image, i, j,
                    (SDL_MapRGB(image->format, 74, 85, 252)));
                array_road_major[i][j] = 1;
                continue;
              }

              // Road minor
              if(h == 0 && s == 0 && v == 1)
              {
                BMP_Put_Pixel(image, i, j,
                    (SDL_MapRGB(image->format, 252, 211, 117)));
                array_road_minor[i][j] = 1;
                continue;
              }

              // White colors
              if(s < 0.17 && v > 0.87 && h > 45)
              {
                continue;
              }

              
              // Brown colors
              if(h > 30 && h < 49.5 && v > 0.60 && ( s > 0.197 || (s > 0.151 && h > 43 && h < 46.5 && s < 0.16) || (s > 0.14 && h > 47 && s < 0.158) || (s > 0.13 && h > 53) || (s > 0.1615 && h > 37 && v > 0.60)))
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
              else if(r == r2 && g == g2 && b == b2)
              {
                BMP_Put_Pixel(image, i, j,
                    (SDL_MapRGB(image->format, 255, 255, 255)));
                array_road_major[i][j] = 1;
              }
              else if(r == r3 && g == g3 && b == b3)
              {
                BMP_Put_Pixel(image, i, j,
                    (SDL_MapRGB(image->format, 0, 255, 0)));
                array_river[i][j] = 1;
              }
            }
        }
    }

    SDL_Surface *lines = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);
    
    rebuilt_lines(lines, &array_topo, bp, tab, h);
    sampling(&bp, image->w, image->h);

    SDL_Surface *pic_river = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);

    rebuilt_river(pic_river, array_river);
    
    SDL_Surface *pic_road_major = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);

    SDL_Surface *pic_road_minor = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);

    SDL_Surface *pic_topo = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);

    SDL_Surface *pic_trail = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);

    SDL_Surface *pic_name = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);

    //bmp_white(pic_river);
    bmp_white(pic_road_major);
    bmp_white(pic_road_minor);
    bmp_white(pic_topo);
    bmp_white(pic_trail);
    bmp_white(pic_name);

    bmp_create(pic_river, array_river, "river.bmp");
    bmp_create(pic_road_major, array_road_major, "road_major.bmp");
    bmp_create(pic_road_minor, array_road_minor, "road_minor.bmp");
    bmp_create(pic_topo, array_topo, "topo.bmp");
    bmp_create(pic_trail, array_trail, "trail.bmp");
    bmp_create(pic_trail, array_name, "name.bmp");

    SDL_Surface *image_texture;
    image_texture = SDL_LoadBMP("Pictures/texture.bmp");
    map_create_texture(image_texture, array_river, array_trail, array_road_major,
        array_road_minor, image->w, image->h);

    // Free Surface
    free(array);
    SDL_FreeSurface(pic_river);
    SDL_FreeSurface(pic_road_major);
    SDL_FreeSurface(pic_road_minor);
    SDL_FreeSurface(pic_topo);
    SDL_FreeSurface(pic_trail);
    SDL_FreeSurface(pic_name);
    SDL_UnlockSurface(image);
    SDL_SaveBMP(image, "Pictures/Results/ign.bmp");
}

