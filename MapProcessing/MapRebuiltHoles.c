#include "MapRebuiltHoles.h"
#include <stdio.h>
#include <stdlib.h>
VEC(list)

//print list of end_pts
void print_list(vector_list *end)
{
  struct end_pts prnt;
  for (size_t i = 0; i<end->element_count; i++)
  {
	prnt = end->data[i];
	printf("x: %i, y: %i\n",prnt.x, prnt.y);
  }
}


//test if a pixel correspond to filter
int is_in_filter(SDL_Surface *image, int x, int y)
{
  int filter[8][9]=
  {
	{0,0,0,-1,1,-1,1,1,1},
	{-1,0,0,1,1,0,-1,1,-1},
	{1,-1,0,1,1,0,1,-1,0},
	{-1,1,-1,1,1,0,-1,0,0},
	{1,1,1,-1,1,-1,0,0,0},
	{-1,1,-1,0,1,1,0,0,-1},
	{0,-1,1,0,1,1,0,-1,1},
	{0,0,-1,0,1,1,-1,1,-1},
  };
  int fd_ngh[9][2] = {{-1,-1},{0,-1},{1,-1},
	  	    {-1,0},{0,0},{1,0},
		    {-1,1},{0,1},{1,1}};
  int neigh[9] = {0,0,0,0,0,0,0,0,0};
  for (int i = 0; i<9; i++)
  {
	if (x+fd_ngh[i][0]>=0 && x+fd_ngh[i][0]<image->w &&
			y+fd_ngh[i][1]>=0 && y+fd_ngh[i][1]<image->h)
		neigh[i]=is_black(image, x+fd_ngh[i][0], y+fd_ngh[i][1]);
  }
  int corr;;
  for (int i = 0; i<8; i++)
  {
	corr = 0;
	for(int j = 0; j<9; j++)
	{
		if (filter[i][j] == -1 || filter[i][j] == neigh[j])
			corr++;
		else
			break;
	}
	if (corr == 9)
		return 1;
  }
  return 0;
}

//thins lines
void thinning(SDL_Surface *image)
{
  int i, j;
  i = 0;
  while (i<image->w)
  {
	j = 0;
	while (j<image->h)
	{
		if (is_black(image,i,j) == 1 &&
				is_in_filter(image, i, j) == 1)
		{
			BMP_Put_Pixel(image, i, j, 
				(SDL_MapRGB(image->format,255,255,255)));
			if (i-2 < 0)
				i = 0;
			else
				i -= 2;
			if (j-2 < 0)
				j = 0;
			else
				j -= 2;
		}
		j++;
	}
	i++;
  }
}

//delete points that are alone (from old writing)
void clean_point(SDL_Surface *image)
{
  int sum;
  for (int i = 0; i< image->w; i++)
  {
	for (int j = 0; j<image->h; j++)
	{
		if (is_black(image, i, j) == 1)
		{
			sum = 0;
			for (int k = i-2; k<i+3; k++)
			{
				for (int l = j-2; l<j+3; l++)
				{
					if (k>=0 && k<image->w && l>=0 
							&& l<image->h)
					{
						if (is_black(image,k,l)==1)
							sum ++;
					}
				}
			}
			if (sum < 2)
				BMP_Put_Pixel(image, i, j, 
					(SDL_MapRGB(image->format,255,255,255)));
		}
	}
  }
}



//search end of a line
void rec_moore(SDL_Surface *image, int **mark, int clock[8][2],
		int x, int y, int i, vector_list *end_list)
{
  mark[x][y] += 1;
  int new_x, new_y, i2;
  int end = 1;
  for (int j = 0; j<8 && end==1; j++)
  {
	i2 = (i+j)%8;
	new_x = x+clock[i2][0];
	new_y = y+clock[i2][1];
	if (new_x>=0 && new_x<image->w && new_y>=0 && new_y<image->h)
	{
		if (j == 6)
  		{
			BMP_Put_Pixel(image, x, y, (SDL_MapRGB(image->format,255,0,0)));
			struct end_pts nw = (struct end_pts){.x = x, .y = y, .state = 0};
			push_back_for_list(end_list, nw);
  		}
		if (mark[new_x][new_y] > 1)
			end = 0;
		else
		{
			if (is_black(image,new_x,new_y) == 1)
			{
				if (i2%2 == 0)
					rec_moore(image, mark, clock,
						new_x,new_y, (i2+6)%8, end_list);
				else
					rec_moore(image, mark, clock,
						new_x, new_y, (i2+5)%8,end_list);
				end = 0;
			}
		}
	}
	else
	{
		end = 0;
		if (mark[x][y] < 2)
			rec_moore(image, mark, clock, x, y, (i2+3)%8, end_list);
	}
  }
}

void moore(SDL_Surface *image, vector_list *end_list)
{
  int **mark = NULL;
  mark = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  {
	mark[i] = (int*)calloc(image->h, sizeof(int*));
  }
  int clock[8][2] =
  {
	{0,1},
	{-1,1},
	{-1,0},
	{-1,-1},
	{0,-1},
	{1,-1},
	{1,0},
	{1,1},
  };
  for (int i = 0; i<image->w; i++)
  {
	for (int j = 0; j<image->h; j++)
	{
		if (mark[i][j] < 2 && is_black(image, i ,j) == 1)
			rec_moore(image, mark, clock, i, j, 0, end_list);
	}
  }
  for (int i=0; i<image->w; i++)
	free(mark[i]);
  free(mark);
}

void rebuilt_lines(SDL_Surface *image)
{
  SDL_LockSurface(image);
  clean_point(image);
  thinning(image);
  vector_list end_list = vector_of_list(0);
  moore(image, &end_list);
  print_list(&end_list);
  delete_vec(end_list);
  //BMP_Draw_Line(image,0,0,150,300, (SDL_MapRGB(image->format,255,0,0)));
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, "Pictures/Results/holes.bmp");
}
