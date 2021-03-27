#ifndef MAPREBUILTHOLES_H
#define MAPREBUILTHOLES_H

#include "vector.h"
#include "tools.h"
struct end_pts {
	int x;
	int y;
	int state;
	int link_x;
	int link_y;
};
typedef struct end_pts list;
VEC_header(list);

void print_list(vector_list *end);
int is_black(SDL_Surface *image, int x, int y);
int is_in_filter(SDL_Surface *image, int x, int y);
void thinning(SDL_Surface *image);
void clean_point(SDL_Surface *image);
void rec_moore(SDL_Surface *image, int **mark, int clock[8][2],
    int x, int y, int i, vector_list *end_list);
void moore(SDL_Surface *image, vector_list *list);
void euclidian(SDL_Surface *image, vector_list *pts, int x, int y);
void link_pts(SDL_Surface *image, vector_list *pts);
void white_black(SDL_Surface *image);
void rebuilt_lines(SDL_Surface *image);

#endif
