#include "MapRebuiltHoles.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
VEC(list)

//print list of end_pts
void print_list(vector_list *end)
{
  struct end_pts prnt;
  for (size_t i = 0; i<end->element_count; i++)
  {
	prnt = end->data[i];
	printf("x: %i, y: %i, state: %i\n",prnt.x, prnt.y, prnt.state);
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
  int test = 0;
  for (int j = 0; j<8 && end==1; j++)
  {
	i2 = (i+j)%8;
	new_x = x+clock[i2][0];
	new_y = y+clock[i2][1];
	if (new_x>=0 && new_x<image->w && new_y>=0 && new_y<image->h)
	{
		if (j == 6)
  		{
  			for (size_t k=0; k<end_list->element_count; k++)
  			{
  				if (end_list->data[k].x == x && end_list->data[k].y == y)
  					test = 1;
  			}
  			if (test == 0)
  			{
				BMP_Put_Pixel(image, x, y, (SDL_MapRGB(image->format,255,0,0)));
				struct end_pts nw = (struct end_pts){.x = x, .y = y, \
				.state = 0, .link_x = -1, .link_y = -1};
				push_back_for_list(end_list, nw);
			}
			test = 0;
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

//find the point of the list which is closer to (x,y) point and draw the line
void euclidian(SDL_Surface *image, vector_list *pts, int x, int y)
{
  float min_d, new_d;
  size_t mini;
  min_d = image->w * image->h;
  struct end_pts pt;
  for (size_t i = 0; i<pts->element_count; i++)
  {
	pt = pts->data[i];
	if ((pt.x != x || pt.y != y) && (pt.link_x != x || pt.link_y != y))
	{
		new_d = sqrt(((pt.x-x)*(pt.x-x))+((pt.y-y)*(pt.y-y)));
		if (i >= pts->element_count-4)
			new_d *= 2;
		if (new_d >1 && new_d < min_d)
		{
			min_d = new_d;
			mini = i;
		}
	}
  }
  pts->data[mini].state = 1;
  pts->data[mini].link_x = x;
  pts->data[mini].link_y = y;
  BMP_Draw_Line(image,x,y,pts->data[mini].x,pts->data[mini].y, (SDL_MapRGB(image->format,0,0,0)));
}

//link points which are end of lines between them or the edge
void link_pts(SDL_Surface *image, vector_list *pts)
{
  struct end_pts pt;
  for (size_t i = 0; i<pts->element_count; i++)
  {
  	pt =  pts->data[i];
  	if (pt.state == 0)
  	{
  		pts->data[i].state = 1;
  		struct end_pts nw1 = (struct end_pts){.x = 0, .y = pt.y, .state = 0};
		push_back_for_list(pts, nw1);
		struct end_pts nw2 = (struct end_pts){.x = image->w-1, .y = pt.y, .state = 0};
		push_back_for_list(pts, nw2);
		struct end_pts nw3 = (struct end_pts){.x = pt.x, .y = 0, .state = 0};
		push_back_for_list(pts, nw3);
		struct end_pts nw4 = (struct end_pts){.x = pt.x, .y = image->h-1, .state = 0};
		push_back_for_list(pts, nw4);
  		euclidian(image, pts, pt.x, pt.y);
  		pts->element_count -= 4;
  	}
  }
}

//transform the image to white and black image
void white_black(SDL_Surface *image)
{
  for (int i = 0; i<image->w; i++)
  {
  	for (int j = 0; j<image->h; j++)
  	{
  		if (is_black(image, i, j) == 1)
  			BMP_Put_Pixel(image, i, j, (SDL_MapRGB(image->format,0,0,0)));
  	}
  }
}

void rebuilt_lines(SDL_Surface *image)
{
  SDL_LockSurface(image);
  white_black(image);
  clean_point(image);
  thinning(image);
  vector_list end_list = vector_of_list(0);
  moore(image, &end_list);
  link_pts(image, &end_list);
  //print_list(&end_list);
  delete_vec(end_list);
  SDL_UnlockSurface(image);
  SDL_SaveBMP(image, "Pictures/Results/holes.bmp");
}
