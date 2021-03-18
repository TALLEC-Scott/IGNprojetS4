#ifndef MAPCOLORISATION_H
#define MAPCOLORISATION_H

#include "tools.h"

// Point
struct point {
  int x;
  int y;
};

// Queue
struct queue {
  struct point* *items;
  int f;
  int r;
};

// Queue
struct queue_int {
  int *data;
  int f;
  int r;
};

void Map_Colorisation(SDL_Surface *image);
int map_dfs_finder(SDL_Surface *image, int x, int y, int label,
int **tab, int first);
int map_end(int **tab, int x, int y, int h, int w);
void dfs(SDL_Surface *image, int x, int y, int **tab, int label);
void bfs_test(SDL_Surface *image, int x, int y, int label,
    int **tab, int *res);
struct queue* create_queue();
void enqueue(struct queue* q, struct point* p, int *size);
struct point* dequeue(struct queue* q);
int is_empty(struct queue* q);
void clean(int **tab, int w, int h);
int* map_elevation(SDL_Surface *image, int **tab, int **h, int label,
    int elevation);
int dfs_elevation(SDL_Surface *image, int x, int y, int **tab, int label,
    int next);
void clean_label(int **tab, int w, int h, int label);
int bfs_elevation(SDL_Surface *image, int x, int y, int label,
    int **tab, int **h2);

// Queue INT
int dequeue_int(struct queue_int* q);
void enqueue_int(struct queue_int* q, int i, int *size);
int is_empty_int(struct queue_int *q);
struct queue_int* create_queue_int();

#endif
