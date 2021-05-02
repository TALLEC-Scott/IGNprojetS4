#ifndef MAPCOLORISATION_H
#define MAPCOLORISATION_H

#include "tools.h"
#include <err.h>

// Point
struct point {
  int x;
  int y;
};

// Queue point
struct queue {
  struct point val;
  struct queue *next;
};

// Queue int
struct queue_int {
  int val;
  struct queue_int *next;
};



void Map_Colorisation(SDL_Surface *image, int **bp, int **tab, int **h);
int map_dfs_finder(SDL_Surface *image, int x, int y, int label,
int **tab, int first);
int map_end(int **tab, int x, int y, int h, int w);
void dfs(SDL_Surface *image, int x, int y, int **tab, int label, int **bp);
void bfs_test(SDL_Surface *image, int x, int y, int label,
    int **tab, int *res, int **bp);
void clean(int **tab, int w, int h);
int* map_elevation(SDL_Surface *image, int **tab, int **h, int label,
    int elevation, int* size);
void dfs_elevation(SDL_Surface *image, int x, int y, int **tab, int label,
    int **h, int elevation, int *new_label, int *size);
void clean_label(int **tab, int w, int h, int label);
void bfs_elevation(SDL_Surface *image, int x, int y, int label,
    int **tab, int **h2, int* list, int* size_2i, int elevation, int total);
void map_elevation_colorize(int **h, int **tab, int label, int elevation,
    int w, int h2);
void mark(int **tab, int label, int w, int h);
int is_present(int* list, int size, int x);
void remove_label(SDL_Surface *image, int **tab, int w, int h, int label,
    int **bp, int label2);

// Queue int
struct queue_int* dequeue_int(struct queue_int* start, int *pval);
struct queue_int* enqueue_int(struct queue_int* start, int val);
void  empty_int(struct queue_int** pstart);
int is_empty_int(struct queue_int* start);

// Queue point
struct queue* enqueue(struct queue* start, struct point val);
struct queue* dequeue(struct queue* start, struct point *pval);
void empty(struct queue** pstart);
int is_empty(struct queue* start);

void map_remplace_label(int **h22, int **tab, int w, int h, int label_old,
    int new);
void map_set_altitude(int **h2, int **tab, int x, int y, int new,
    int w, int h, int manual);
void bfs_set_altitude(int x, int y, int w, int h, int **tab, int **tab2, int **h2,
int label, int label_old, struct point *list, int *size);
int is_present2(struct point *list, int size, int x);



#endif
