#include "drawing_tools.h"

/* Draws Points from 2D array M (currently lists of list) */
void Draw_Points(int **bp, SDL_Surface *image)
{
    glBegin(GL_POINTS);
    glColor3f(0.0f, 1.0f, 0.5f);

    for (int i = 0; i < image->w; i++)
    {
        for (int j = 0; j < image->h; j++)
        {
            if (bp[i][j] != 0)
            {
                glVertex3f(((float)j / (float)image->w), ((float)i / (float)image->h),
                           (float)bp[i][j] / 1500.);
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
