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
VEC_header(list)

struct image_pict {
	int** pict;
	int w;
	int h;
};

void print_list(vector_list *end);
int is_black(SDL_Surface *image, int x, int y);
int is_in_filter(struct image_pict *image, int x, int y, int is_line);
void thinning(struct image_pict *image, int is_line);
void enlarge_riv(struct image_pict *image);
/*void rec_moore(struct image_pict *image, int **mark, int clock[8][2],
    int x, int y, int i, vector_list *end_list, int first);
void moore(struct image_pict*image, vector_list *list);*/
void neigh(struct image_pict *image, vector_list *end_list);
int is_looped(struct image_pict *image, int x1, int y1, int x2, int y2,int cpt, int **mark);
int is_linked(struct image_pict *image, int x1, int y1, int x2, int y2);
int euclidian(struct image_pict *image, vector_list *pts, int x, int y, size_t i_src, size_t i_dst, float i_mini, int max_dist);
void link_pts(struct image_pict *image, vector_list *pts, int max_dist);
void rebuilt_lines(SDL_Surface *image, int **tab, int **bp, int **tab2, int **h);
void rebuilt_river(SDL_Surface *image, int **tab);
void array_Draw_Line(struct image_pict *image, int x1, int y1, int x2, int y2, int pixel);
SDL_Surface* BMP_To_BW(SDL_Surface *image);

#endif
