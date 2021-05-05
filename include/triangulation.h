#ifndef TRIANGULATION_H
#define TRIANGULATION_H

#define EPSILON 0.000001

#include <gtk/gtk.h>
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
   int p1,p2,p3;
} ITRIANGLE;
typedef struct {
   int p1,p2;
} IEDGE;
typedef struct {
   double x,y,z;
} XYZ;


int XYZCompare(const void *v1,const void *v2);
int Triangulate(int nv,XYZ *pxyz,ITRIANGLE *v,int *ntri);




#endif
