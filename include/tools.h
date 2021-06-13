#ifndef TOOLS_H
#define TOOLS_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include "MapRebuiltHoles.h"
#include "app.h"
#include "Open_GL_exec.h"

Uint32 BMP_Get_Pixel(SDL_Surface *image, int x, int y);
void BMP_Put_Pixel(SDL_Surface *image, int x, int y, Uint32 pixel);
void bmp_create(SDL_Surface *image, int **array, char *name);
void BMP_Test(SDL_Surface *image, int **tab);
double Max(double r, double g, double b);
double Min(double r, double g, double b);
void RGB_To_HSV(double r, double g, double b, double array[3]);
void bmp_white(SDL_Surface *image);
void bmp_test2(SDL_Surface *image, int **tab);
int is_black(SDL_Surface *image, int x, int y);
size_t len_array_int(int *array);
void bmp_test3(SDL_Surface *image, int **tab, int **h);
void process_array(int **array, int **h2, int h, int w);
void search_points(_3D_Coord *max_point, _3D_Coord *min_point, int **bp, SDL_Surface *image);


typedef struct _2D_arrays {

	size_t cols;
	size_t rows;
	int *data[]; /*1D array of size_t type */
} _2D_array;

int get_2D_array_x_y (_2D_array *M, size_t x, size_t y);
void set_2D_array_x_y (_2D_array *M, size_t x, size_t y, size_t new_val);
void load_image(Image *image, char *file);
#endif
