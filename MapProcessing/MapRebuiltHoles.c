#include "MapRebuiltHoles.h"
#include "MapColorisation.h"
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
int is_in_filter(struct image_pict *image, int x, int y, int is_line)
{
  int filter[31][25]=
  {
  	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,0,1,0,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
  	{0,0,0,0,0,0,-1,-1,-1,0,0,-1,1,-1,0,0,-1,-1,-1,0,0,0,0,0,0},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,-1,1,-1,-1,-1,1,1,1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,0,0,-1,-1,1,1,0,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,1,-1,0,-1,-1,1,1,0,-1,-1,1,-1,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,1,1,0,-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,1,1,1,-1,-1,-1,1,-1,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,-1,1,-1,-1,-1,0,1,1,-1,-1,0,0,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,-1,1,-1,-1,0,1,1,-1,-1,0,-1,1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,-1,-1,-1,0,1,1,-1,-1,-1,1,-1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,0,-1,-1,0,1,1,1,-1,0,1,0,-1,-1,-1,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,1,0,-1,0,1,1,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,0,1,1,-1,-1,0,0,1,0,-1,-1,-1,1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,0,1,0,-1,-1,0,1,1,1,-1,-1,-1,0,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,0,1,0,-1,1,1,1,0,-1,-1,0,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,-1,1,1,0,-1,0,1,0,0,-1,-1,1,-1,-1,-1},
	{-1,1,-1,-1,-1,0,1,0,0,-1,-1,1,1,0,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,0,-1,-1,-1,1,1,1,0,-1,-1,0,1,0,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,0,-1,-1,-1,1,1,1,-1,-1,0,1,0,-1,-1,0,1,0,-1,-1,-1,0,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,1,-1,0,1,1,1,0,-1,0,0,1,-1,-1,-1,-1,-1,-1},
	{-1,-1,0,-1,-1,-1,0,1,0,-1,-1,0,1,0,-1,-1,1,1,1,-1,-1,-1,0,-1,-1},
	{-1,-1,-1,-1,-1,-1,1,0,0,-1,0,1,1,1,0,-1,1,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,-1,1,-1,0,1,1,-1,-1,0,0,1,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,1,-1,0,0,1,-1,-1,0,1,1,1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,-1,0,0,0,-1,1,1,1,0,-1,1,0,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,1,0,0,0,0,0,1,1,-1,0,1,0,0,0,0,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,-1,1,-1,0,0,-1,-1,1,1,0,-1,-1,1,0,0,-1,-1,-1,-1,-1,-1},
	{-1,-1,-1,-1,1,0,0,0,0,1,0,-1,1,1,-1,0,0,0,0,1,-1,-1,-1,-1,-1},
	{0,0,0,1,0,0,0,0,0,1,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0},
	{1,-1,-1,-1,-1,1,1,0,0,-1,1,0,1,0,-1,-1,0,0,0,-1,-1,-1,-1,-1,-1},
	{0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,1,0}
  };
  int fd_ngh[25][2] = {{-2,-2},{-1,-2},{0,-2},{1,-2},{2,-2},
  		       {-2,-1},{-1,-1},{0,-1},{1,-1},{2,-1},
	  	       {-2,0},{-1,0},{0,0},{1,0},{2,0},
		       {-2,1},{-1,1},{0,1},{1,1},{2,1},
		       {-2,2},{-1,2},{0,2},{1,2},{2,2}};
  int corr, test;
  for (int i = 0; i<31; i++)
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
	{
		test = 1;
		if (is_line == 1 && i == 1 && x-3>=0 && x+3<image->w && y-3>=0 && y+3<image->h)
		{
			for (int m = x-3; m<=x+3; m++)
			{
				for (int n = y-3; n<=y+3; n++)
				{
					if ((m == x-3 || m == x+3 || n == y-3 || n == y+3)
						&& (image->pict[m][n] != 0))
						test = 0;
				}
			}
		}
		if (test)
			return 1;
	}
  }
  return 0;
}

//thins lines
void thinning(struct image_pict *image, int is_line)
{
  int i, j;
  i = 0;
  while (i<image->w)
  {
	j = 0;
	while (j<image->h)
	{
		if (image->pict[i][j] != 0 &&
				is_in_filter(image, i, j, is_line) == 1)
		{
			image->pict[i][j] = 0;
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


void enlarge_riv(struct image_pict *image)
{
  for (int i = 0; i<image->w; i++)
  {
  	for (int j = 0; j<image->h; j++)
  	{
  		if (image->pict[i][j] == 1)
  		{
  			if (i>0)
  			{
  				image->pict[i-1][j] = 3;
  				if (j>0)
  					image->pict[i-1][j-1] = 3;
  				if (j<image->h-1)
  					image->pict[i-1][j+1] = 3;
  			}
  			if (j>0)
  			{
  				image->pict[i][j-1] = 3;
  				if (i<image->w-1)
  					image->pict[i+1][j-1] = 3;
  			}
  			if (i<image->w-1)
  			{
  				image->pict[i+1][j] = 3;
  				if (j<image->h-1)
  					image->pict[i+1][j+1] = 3;
  			}
  			if (j<image->h-1)
  				image->pict[i][j+1] = 3;
  		}
  	}
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
  int count, edge, last, new_x, new_y;
  for (int x = 0; x<image->w; x++)
  {
  	for (int y = 0; y<image->h; y++)
  	{
  		if (image->pict[x][y] != 0)
  		{
  		count = 0;
  		edge = 0;
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
			{
				count ++;
				edge ++;
			}
		}
		if ((edge > 0 && count-edge == 0) || count < 2)
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
  if (cpt > 10)
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
  if (abs(x2-x1) <= 1 && abs(y2-y1) <= 1 && image->pict[x1][y1] != 0)
  	return 1;
  int d, dx, dy, incr1, incr2, incr_x, incr_y, x, y;
  int link = 0;
  if (x1==981 && y1==651 && x2==986 && y2==649)
	printf("start\n");
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
  	if (x1==981 && y1==651 && x2==986 && y2==649)
		printf("good if\n");
  	incr_y = y2 > y1 ? 1 : -1;
  	dx = x2 - x1;
  	dy = abs(y2 - y1);
  	d = 2*dy - dx;
  	incr1 = 2 * (dy-dx);
  	incr2 = 2 * dy;
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
  		if (x1==981 && y1==651 && x2==986 && y2==649)
			printf("x: %i, y: %i, value: %i, d: %i, incr2: %i, incr1: %i\n",x,y,image->pict[x][y],d,incr2,incr1);
  		if (x>=0 && x<image->w && y>=0 && y<image->h)
  		{
  			if (image->pict[x][y] != 0)
  				link ++;
  		}
  	}
  }
  if (x1==981 && y1==651 && x2==986 && y2==649)
	printf("link: %i\n",link);
  if (link > 1)
  	return 1;
  return 0;
}

//find the point of the list which is closer to (x,y) point and draw the line
int euclidian(struct image_pict *image, vector_list *pts, int x, int y, size_t i_src, size_t i_dst, float i_mini, int max_dist)
{
  float min_d, new_d;
  size_t mini = i_src;
  min_d = i_mini;
  struct end_pts pt;
  size_t len = pts->element_count;
  if (x > 963 && x < 990 && y > 645 && y < 700)
  	printf("x: %i, y: %i, min dist: %f\n",x,y,min_d);
  for (size_t i = 0; i<len+4; i++)
  {
  	if (i >= len)
  	{
  		if (i == len)
  		{
  			struct end_pts nw1 = (struct end_pts){.x = 0, .y = y, .state = 0};
			push_back_for_list(pts, nw1);
			struct end_pts nw2 = (struct end_pts){.x = image->w-1, .y = y, .state = 0};
			push_back_for_list(pts, nw2);
			struct end_pts nw3 = (struct end_pts){.x = x, .y = 0, .state = 0};
			push_back_for_list(pts, nw3);
			struct end_pts nw4 = (struct end_pts){.x = x, .y = image->h-1, .state = 0};
			push_back_for_list(pts, nw4);
  		}
  		pt = pts->data[i];
  		if (pt.state == 0 && (pt.x != x || pt.y != y))
		{
			new_d = sqrt(((pt.x-x)*(pt.x-x))+((pt.y-y)*(pt.y-y))) * 5;
			if (new_d > 1 && new_d*2 < max_dist && new_d <= min_d && is_linked(image,pt.x,pt.y,x,y) == 0)
			{
				int **mark = NULL;
	  			mark = (int**)calloc(image->w, sizeof(int*));
	 			for(int j = 0; j < image->w; j++)
					mark[j] = (int*)calloc(image->h, sizeof(int*));
	  			if (is_looped(image, pt.x,pt.y,x,y, 0, mark) == 0)
				{
					min_d = new_d;
					mini = i;
				}
				for (int j=0; j<image->w; j++)
					free(mark[j]);
	  			free(mark);
			}
		}
  	}
  	else
  	{
		pt = pts->data[i];
		if (pt.state == 0 && (pt.x != x || pt.y != y))
		{
			new_d = sqrt(((pt.x-x)*(pt.x-x))+((pt.y-y)*(pt.y-y)));
			if (x > 963 && x < 990 && y > 645 && y < 700 && pt.x > 963 && pt.x < 990 && pt.y > 645 && pt.y < 700)
  				printf("x2: %i, y2: %i, dist: %f\n",pt.x,pt.y,new_d);
			if (new_d > 1 && new_d < max_dist && new_d <= min_d && is_linked(image,pt.x,pt.y,x,y) == 0)
			{
				int **mark = NULL;
	  			mark = (int**)calloc(image->w, sizeof(int*));
	 			for(int j = 0; j < image->w; j++)
					mark[j] = (int*)calloc(image->h, sizeof(int*));
	  			if (is_looped(image, pt.x,pt.y,x,y, 0, mark) == 0)
				{
					if (i == i_dst || euclidian(image, pts, pt.x, pt.y, i, i_src, new_d, max_dist) == 1)
					{
						min_d = new_d;
						mini = i;
					}
				}
				for (int j=0; j<image->w; j++)
					free(mark[j]);
	  			free(mark);
			}
		}
	}
  }
  
  if (mini == i_src || min_d == image->w * image->h || pts->data[mini].state == 1 || pts->data[i_src].state == 1)
  	image->pict[x][y] = 0;
  else
  {
  	if (mini == i_dst)
  	{
  		pts->element_count -= 4;
  		return 1;
  	}
  	if (x > 963 && x < 990 && y > 645 && y < 700)
  		printf("x: %i, y: %i, new_x: %i, new_y: %i, state1: %i, state2: %i\n",x,y,pts->data[mini].x,pts->data[mini].y,pts->data[mini].state, pts->data[i_src].state);
  	pts->data[mini].state = 1;
  	pts->data[i_src].state = 1;
  	array_Draw_Line(image,x,y,pts->data[mini].x,pts->data[mini].y, 1);
  }
  pts->element_count -= 4;
  return 0;
}

//link points which are end of lines between them or the edge
void link_pts(struct image_pict *image, vector_list *pts, int max_dist)
{
  struct end_pts pt;
  for (size_t i = 0; i<pts->element_count; i++)
  {
  	pt =  pts->data[i];
  	if (pt.state == 0)
  		euclidian(image, pts, pt.x, pt.y, i, -1, image->w * image->h, max_dist);
  }
}


void rebuilt_lines(SDL_Surface *image, int **tab, int **bp, int **tab2, int **h)
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
  thinning(pict, 1);
  vector_list end_list = vector_of_list(0);
  neigh(pict, &end_list);
  SDL_Surface *pic_neigh = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);
  bmp_create(pic_neigh, pict->pict, "neigh.bmp");
  int max = 5;
  while (max > 0 && end_list.element_count != 0)
  {
  	link_pts(pict, &end_list, 100);
  	//print_list(&end_list);
  	end_list.element_count = 0;
  	neigh(pict, &end_list);
  	max --;
  }
  delete_vec(end_list);
  thinning(pict, 1);
  bmp_create(image, pict->pict, "holes.bmp");
  Map_Colorisation(image, bp, tab2, h);
  for(int i = 0; i < pict->w; i++)
  	free(pict->pict[i]);
  free(pict->pict);
  free(pict);
  SDL_UnlockSurface(image);
  SDL_FreeSurface(image);
}

void rebuilt_river(SDL_Surface *image, int **tab)
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
  thinning(pict, 0);
  vector_list end_list = vector_of_list(0);
  neigh(pict, &end_list);
  SDL_Surface *pic_neigh = SDL_CreateRGBSurface(0, image->w, image->h,
        image->format->BitsPerPixel, image->format->Rmask,
        image->format->Gmask, image->format->Bmask, image->format->Amask);
  bmp_create(pic_neigh, pict->pict, "neigh_riv.bmp");
  link_pts(pict, &end_list, 20);
  end_list.element_count = 0;
  neigh(pict, &end_list);
  link_pts(pict, &end_list, 20);
  end_list.element_count = 0;
  delete_vec(end_list);
  enlarge_riv(pict);
  bmp_create(image, pict->pict, "holes_riv.bmp");
  for(int i = 0; i < pict->w; i++)
  	free(pict->pict[i]);
  free(pict->pict);
  free(pict);
  SDL_UnlockSurface(image);
  //SDL_FreeSurface(image);
}
