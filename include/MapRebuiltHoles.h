#ifndef MAPREBUILTHOLES_H
#define MAPREBUILTHOLES_H

#include "vector.h"
#include "tools.h"
struct end_pts {
	int x;
	int y;
	int state;
};
typedef struct end_pts list;
VEC_header(list);
struct image_pict {
	int** pict;
	int w;
	int h;
};

void print_list(vector_list *end);
int is_black(SDL_Surface *image, int x, int y);
int is_in_filter(struct image_pict *image, int x, int y);
void thinning(struct image_pict *image);
void rec_moore(struct image_pict *image, int **mark, int clock[8][2],
    int x, int y, int i, vector_list *end_list, int first);
void moore(struct image_pict*image, vector_list *list);
int is_looped(struct image_pict *image, int x1, int y1, int x2, int y2,int cpt, int **mark);
int is_linked(struct image_pict *image, int x1, int y1, int x2, int y2);
void euclidian(struct image_pict *image, vector_list *pts, int x, int y);
void link_pts(struct image_pict *image, vector_list *pts);
void rebuilt_lines(SDL_Surface *image);

#endif
