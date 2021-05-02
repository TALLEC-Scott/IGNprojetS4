#include "drawing_tools.h"

/* Extrapolates the size biggest of the dim(height or width), so that when we "norm" we don't get numbers >1 and at the same point
don't lose the x to y proportion */
float biggest_dim_size(SDL_Surface *image){
    if (image->w > image->h){
        return image->w;
    }
    //else
    return image->h; 


}
/*
unsigned int VBO;
glGenBuffers(1, &VBO); 
*/


/* Tallest point on the map */
int biggest_height(SDL_Surface *image, int **bp){
    int max = 0;
    for (int i = 0; i < image->w; i++)
    {
        for (int j = 0; j < image->h; j++){
            if (bp[i][j]> max)
                max = bp[i][j];
        }
    }
    return max;
}
    
/* Draws Points from 2D array M (currently lists of list) */
void Draw_Points(int **bp, SDL_Surface *image)
{
    glBegin(GL_POINTS);
    glColor3f(0.0f, 1.0f, 0.5f);
    int max = biggest_height(image, bp);
    float max_dim_size = biggest_dim_size(image);

    for (int i = 0; i < image->w; i++)
    {
        for (int j = 0; j < image->h; j++)
        {
            if (bp[i][j] != 0)
            {
                glVertex3f(((float)i / max_dim_size)*2 -image->w/max_dim_size, ((float)j / max_dim_size)*2 +(image->h/max_dim_size),
                           (float)bp[i][j] / (float) max);
            }
        }
    }

    glEnd();
}


/* Free the memory allocated to bp after accessing it's data */
void free_bm(int **bp, SDL_Surface *image)
{
    for (int i = 0; i < image->w; i++)
    {

        free(bp[i]);
    }
    free(bp);
}
