#ifndef MAPREBUILTHOLES_H
#define MAPREBUILTHOLES_H

#include "tools.h"
struct end_pts {
	int x;
	int y;
	struct end_pts *next;
	struct end_pts *first;
	struct end_pts *prev;
};

void print_list(struct end_pts *list);
int is_black(SDL_Surface *image, int x, int y);
int is_in_filter(SDL_Surface *image, int x, int y);
void thinning(SDL_Surface *image);
void clean_point(SDL_Surface *image);
void rec_moore(SDL_Surface *image, int **mark, int clock[8][2],
    int x, int y, int i, struct end_pts *list);
void moore(SDL_Surface *image, struct end_pts *list);
void rebuilt_lines(SDL_Surface *image);

#endif
