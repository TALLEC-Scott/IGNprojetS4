#ifndef DRAWING_TOOLS_H
#define DRAWING_TOOLS_H
#include <stdio.h>
#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <GL/glut.h>
#include "triangulation.h"


typedef struct Attributes
{
    int greatest;
    int smallest;
    int nb_points;
} Attributes;

typedef struct _3D_Coord
{
    int x;
    int y;
    int z;
} _3D_Coord;

void Draw_Points(int **bp, SDL_Surface *image);
void Get_Triangles(int **bp, SDL_Surface *image, _3D_Coord arr[]);
void Draw_Triangles(XYZ *p, ITRIANGLE *v, int ntri, int **bp, SDL_Surface *image  );
void free_bm(int **bp, SDL_Surface *image);
Attributes attr(SDL_Surface *image, int **bp);
void Print_Arr_of_Coord(int nb_points, _3D_Coord arr[]);
void List_of_Points(int **bp, SDL_Surface *image,_3D_Coord **res);
#endif
