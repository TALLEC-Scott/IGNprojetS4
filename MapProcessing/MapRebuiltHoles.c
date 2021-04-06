#include "MapRebuiltHoles.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
VEC(list)

//Draw a line between 2 points (x1,y1) and (x2,y2) in BMP file image (algo of Bresenham)
void array_Draw_Line(struct image_pict *image, int x1, int y1, int x2, int y2, int pixel)
{
  int d, dx, dy, incr1, incr2, incr_x, incr_y, x, y;
  if (abs(x2-x1) < abs(y2-y1))
  {
  	if (y1 > y2)
  	{
  		int tmp_x = x1;
  		int tmp_y = y1;
  		x1 = x2;
  		y1 = y2;
  		x2 = tmp_x;
  		y2 = tmp_y;
  	}
  	incr_x = x2 > x1 ? 1 : -1;
  	dy = y2 - y1;
  	dx = abs(x2 - x1);
  	d = 2*dx - dy;
  	incr1 = 2 * (dx-dy);
  	incr2 = 2 * dx;
  	x = x1;
  	y = y1;
  	image->pict[x][y] = pixel;
  	for (y = y1+1; y<=y2; y++)
  	{
  		if (d >= 0)
  		{
  			x += incr_x;
  			d += incr1;
  		}
  		else
  			d += incr2;
  		image->pict[x][y] = pixel;
  	}
  }
  else
  {
  	if (x1 > x2)
  	{
  		int tmp_x = x1;
  		int tmp_y = y1;
  		x1 = x2;
  		y1 = y2;
  		x2 = tmp_x;
  		y2 = tmp_y;
  	}
  	incr_y = y2 > y1 ? 1 : -1;
  	dx = x2 - x1; 
  	dy = abs(y2 - y1);
  	d = 2*dy - dx; 
  	incr1 = 2 * (dy-dx);
  	incr2 = 2 * dy;
  	x = x1;
  	y = y1;
  	image->pict[x][y] = pixel;
  	for (x = x1+1; x<=x2; x++)
  	{
  		if (d >= 0)
  		{
  			y += incr_y;
  			d += incr1;
  		}
  		else
  			d += incr2;
  		image->pict[x][y] = pixel;
  	}
  }
  image->pict[x1][y1] = pixel;
  image->pict[x2][y2] = pixel;
}

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
int is_in_filter(struct image_pict *image, int x, int y)
{
  int filter[23][25]=
  {
  	//{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,0,1,0,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,-1,1,-1,-1,-1,1,1,1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,0,0,-1,-1,1,1,0,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,1,-1,0,-1,-1,1,1,0,-1,-1,1,-1,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,1,1,0,-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,1,1,1,-1,-1,-1,1,-1,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,0,1,1,-1,-1,0,0,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,-1,1,-1,-1,0,1,1,-1,-1,0,-1,1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,-1,-1,-1,0,1,1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,0,-1,-1,0,1,1,1,-1,0,1,0,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,1,-1,-1,0,0,1,0,-1,0,1,1,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,0,1,1,-1,-1,0,0,1,0,-1,-1,-1,1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,0,1,0,-1,-1,0,1,1,1,-1,-1,-1,0,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,0,1,0,-1,1,1,1,0,-1,-1,0,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,1,1,0,-1,0,1,0,0,-1,-1,1,-1,-1,-1},
	{-1,1,-1,-1,-1,0,1,0,0,-1,-1,1,1,0,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,0,-1,-1,-1,1,1,1,0,-1,-1,0,1,0,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,0,-1,-1,-1,1,1,1,-1,-1,0,1,0,-1,-1,0,1,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,1,-1,-1,1,1,1,0,-1,0,0,1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,1,0,-1,-1,0,1,0,-1,-1,1,1,1,-1,-1,-1,0,-1,-1},
	{-1,-1,-1,-1,-1,-1,1,0,0,-1,0,1,1,1,-1,-1,1,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,-1,1,-1,0,1,1,-1,-1,0,0,1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,1,-1,0,0,1,-1,-1,0,1,1,1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,1,1,1,0,-1,1,0,0,0,-1,-1,-1,-1,-1,-1}
  };
  int fd_ngh[25][2] = {{-2,-2},{-1,-2},{0,-2},{1,-2},{2,-2},
  		       {-2,-1},{-1,-1},{0,-1},{1,-1},{2,-1},
	  	       {-2,0},{-1,0},{0,0},{1,0},{2,0},
		       {-2,1},{-1,1},{0,1},{1,1},{2,1},
		       {-2,2},{-1,2},{0,2},{1,2},{2,2}};
  int corr;
  for (int i = 0; i<23; i++)
  {
	corr = 0;
	for(int j = 0; j<25; j++)
	{
		if (x+fd_ngh[j][0]>=0 && x+fd_ngh[j][0]<image->w &&
			y+fd_ngh[j][1]>=0 && y+fd_ngh[j][1]<image->h)
		{
		if (filter[i][j] == -1 || (filter[i][j] == 1 && image->pict[x+fd_ngh[j][0]][y+fd_ngh[j][1]] != 0)
				|| (filter[i][j] == 0 && image->pict[x+fd_ngh[j][0]][y+fd_ngh[j][1]] == 0))
			corr++;
		else
			break;
		}
	}
	if (corr == 25)
		return 1;
  }
  return 0;
}

//thins lines
void thinning(struct image_pict *image)
{
  int i, j;
  i = 0;
  while (i<image->w)
  {
	j = 0;
	while (j<image->h)
	{
		if (image->pict[i][j] != 0 &&
				is_in_filter(image, i, j) == 1)
		{
			image->pict[i][j] = 0;
			if (i-1 < 0)
				i = 0;
			else
				i --;
			if (j-1 < 0)
				j = 0;
			else
				j --;
		}
		j++;
	}
	i++;
  }
}


/*//search end of a line
void rec_moore(struct image_pict *image, int **mark, int clock[8][2],
		int x, int y, int i, vector_list *end_list, int first)
{
  mark[x][y] += 1;
  int new_x, new_y, i2;
  int end = 0;
  int test = 0;
  for (int j = 0; j<8 && (end==0 || (first == 1 && end == 1)); j++)
  {
	i2 = (i+j)%8;
	new_x = x+clock[i2][0];
	new_y = y+clock[i2][1];
	if (new_x>=0 && new_x<image->w && new_y>=0 && new_y<image->h)
	{
		if ((first == 0 && j == 6) || (first == 1 && j == 7))
  		{
  			for (size_t k=0; k<end_list->element_count; k++)
  			{
  				if (end_list->data[k].x == x && end_list->data[k].y == y)
  					test = 1;
  			}
  			if (test == 0)
  			{
  				image->pict[x][y] = 2;
				struct end_pts nw = (struct end_pts){.x = x, .y = y, .state = 0};
				push_back_for_list(end_list, nw);
				
			}
			test = 0;
  		}
		if (mark[new_x][new_y] > 1)
			end ++;
		else
		{
			if (image->pict[new_x][new_y] != 0)
			{
				if (i2%2 == 0)
					rec_moore(image, mark, clock,
						new_x,new_y, (i2+6)%8, end_list,0);
				else
					rec_moore(image, mark, clock,
						new_x, new_y, (i2+5)%8,end_list,0);
				end ++;
			}
		}
	}
	else
	{
		end ++;
		if (mark[x][y] < 2)
			rec_moore(image, mark, clock, x, y, (i2+3)%8, end_list,0);
	}
  }
}

void moore(struct image_pict *image, vector_list *end_list)
{
  int **mark = NULL;
  mark = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
	mark[i] = (int*)calloc(image->h, sizeof(int*));
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
		if (mark[i][j] < 2 && image->pict[i][j] != 0)
			rec_moore(image, mark, clock, i, j, 0, end_list, 1);
	}
  }
  for (int i=0; i<image->w; i++)
	free(mark[i]);
  free(mark);
}*/

void neigh(struct image_pict *image, vector_list *end_list)
{
  int fd_ngh[8][2] = {{-1,-1},{0,-1},{1,-1},{1,0},{1,1},{0,1},{-1,1},{-1,0}};
  int count, last, new_x, new_y;
  for (int x = 0; x<image->w; x++)
  {
  	for (int y = 0; y<image->h; y++)
  	{
  		if (image->pict[x][y] != 0)
  		{
  		count = 0;
  		last = -2;
  		for (int i = 0; i<8; i++)
  		{
  			new_x = x+fd_ngh[i][0];
  			new_y = y+fd_ngh[i][1];
			if (new_x>=0 && new_x<image->w && new_y>=0 && new_y<image->h)
			{
				if (image->pict[new_x][new_y] != 0 && i-last>1)
				{
					count++;
					last = i;
				}
			}
			else
				count++;
		}
		if (count < 2)
  		{
  			image->pict[x][y] = 2;
			struct end_pts nw = (struct end_pts){.x = x, .y = y, .state = 0};
			push_back_for_list(end_list, nw);
  		}
  		}
	}
  }
}

//see if there is a loop between point (x1,y1) and point (x2,y2)
int is_looped(struct image_pict *image, int x1, int y1, int x2, int y2, int cpt, int **mark)
{
  if (cpt > 50)
  	return 0;
  int new_x, new_y;
  int fd_ngh[9][2] = {{-1,-1},{0,-1},{1,-1},
  		      {-1,0},{0,0},{1,0},
  		      {-1,1},{0,1},{1,1}};
  mark[x1][y1] = 1;
  for (int i = 0; i<9; i++)
  {
  	new_x = x1+fd_ngh[i][0];
  	new_y = y1+fd_ngh[i][1];
  	if (new_x == x2 && new_y == y2)
  		return 1;
	if (new_x>=0 && new_x<image->w && new_y>=0 && new_y<image->h)
	{
		if (image->pict[new_x][new_y] != 0 && mark[new_x][new_y] == 0)
			if (is_looped(image,new_x,new_y,x2,y2,cpt+1,mark) == 1)
				return 1;
	}
  }
  return 0;
}

//see if point (x1,y1) is linked to point (x2,y2)
int is_linked(struct image_pict *image, int x1, int y1, int x2, int y2)
{
  int d, dx, dy, incr1, incr2, incr_x, incr_y, x, y;
  int link = 0;
  if (abs(x2-x1) < abs(y2-y1))
  {
  	if (y1 > y2)
  	{
  		int tmp_x = x1;
  		int tmp_y = y1;
  		x1 = x2;
  		y1 = y2;
  		x2 = tmp_x;
  		y2 = tmp_y;
  	}
  	incr_x = x2 > x1 ? 1 : -1;
  	dy = y2 - y1;
  	dx = abs(x2 - x1);
  	d = 2*dx - dy;
  	incr1 = 2 * (dx-dy);
  	incr2 = 2 * dx;
  	x = x1;
  	y = y1;
  	for (y = y1+1; y<=y2 && link<2; y++)
  	{
  		if (d >= 0)
  		{
  			x += incr_x;
  			d += incr1;
  		}
  		else
  			d += incr2;
  		if (image->pict[x][y] != 0)
  			link ++;
  	}
  }
  else
  {
  	if (x1 > x2)
  	{
  		int tmp_x = x1;
  		int tmp_y = y1;
  		x1 = x2;
  		y1 = y2;
  		x2 = tmp_x;
  		y2 = tmp_y;
  	}
  	incr_y = y2 > y1 ? 1 : -1;
  	dx = x2 - x1;
  	dy = abs(y2 - y1);
  	d = 2*dy - dx;
  	incr1 = 2 * (dy-dx);
  	incr2 = 2 * d;
  	x = x1;
  	y = y1;
  	for (x = x1+1; x<=x2 && link<2; x++)
  	{
  		if (d >= 0)
  		{
  			y += incr_y;
  			d += incr1;
  		}
  		else
  			d += incr2;
  		if (x>=0 && x<image->w && y>=0 && y<image->h)
  		{
  			if (image->pict[x][y] != 0)
  				link ++;
  		}
  	}
  }
  if (link > 1)
  	return 1;
  return 0;
}

//find the point of the list which is closer to (x,y) point and draw the line
void euclidian(struct image_pict *image, vector_list *pts, int x, int y)
{
  float min_d, new_d;
  size_t mini;
  min_d = image->w * image->h;
  struct end_pts pt;
  for (size_t i = 0; i<pts->element_count; i++)
  {
	pt = pts->data[i];
	if (pt.x != x || pt.y != y)
	{
		new_d = sqrt(((pt.x-x)*(pt.x-x))+((pt.y-y)*(pt.y-y)));
		if (i >= pts->element_count-4)
			new_d *= 5;
		if (new_d > 1 && ((i >= pts->element_count-4 && new_d < 100) ||new_d < 30) && 
			new_d < min_d && is_linked(image,pt.x,pt.y,x,y) == 0)
		{
			if (i >= pts->element_count-4)
			{
				int **mark = NULL;
  				mark = (int**)calloc(image->w, sizeof(int*));
 				for(int i = 0; i < image->w; i++)
					mark[i] = (int*)calloc(image->h, sizeof(int*));
  				if (is_looped(image, pt.x,pt.y,x,y, 0, mark) == 1)
				{
					min_d = new_d;
					mini = i;
				}
				for (int i=0; i<image->w; i++)
					free(mark[i]);
  				free(mark);
			}
			min_d = new_d;
			mini = i;
		}
	}
  }
  if (min_d == image->w * image->h)
  	image->pict[x][y] = 0;
  else
  {
  	pts->data[mini].state = 1;
  	array_Draw_Line(image,x,y,pts->data[mini].x,pts->data[mini].y, 1);
  }
}

//link points which are end of lines between them or the edge
void link_pts(struct image_pict *image, vector_list *pts)
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


void rebuilt_lines(SDL_Surface *image, int **tab)
{
  SDL_LockSurface(image);
  struct image_pict *pict = malloc(sizeof(struct image_pict));
  pict->pict = (int**)calloc(image->w, sizeof(int*));
  for(int i = 0; i < image->w; i++)
  	pict->pict[i] = (int*)calloc(image->h, sizeof(int*));
  for (int i = 0; i<image->w; i++)
  {
  	for (int j = 0; j<image->h; j++)
  	{
  		if (tab[i][j] == 1)
  			pict->pict[i][j] = 1;
  	}
  }
  pict->w = image->w;
  pict->h = image->h;
  thinning(pict);
  vector_list end_list = vector_of_list(0);
  neigh(pict, &end_list);
  SDL_Surface *pic_neigh = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);
  bmp_create(pic_neigh, pict->pict, "neigh.bmp");
  while (end_list.element_count != 0)
  {
  	link_pts(pict, &end_list);
  	//print_list(&end_list);
  	end_list.element_count = 0;
  	neigh(pict, &end_list);
  }
  delete_vec(end_list);
  bmp_create(image, pict->pict, "holes.bmp");
  for(int i = 0; i < pict->w; i++)
  	free(pict->pict[i]);
  free(pict->pict);
  free(pict);
  SDL_UnlockSurface(image);
}
