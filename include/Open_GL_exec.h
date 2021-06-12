#ifndef OPEN_GL_EXEC_H
#define OPEN_GL_EXEC_H
#include "triangulation.h"
#include "drawing_tools.h"
#include "MapColorisation.h"



void initGL();
void display();
void reshape(GLsizei width, GLsizei height);
void keyboard(unsigned char key, int a, int b);
void SpecialKeys(int key, int a, int b);
/*int execute_function(int argc, char **argv, SDL_Surface *im, int** bps,
    int **river, int **trail, int **road_major, int **road_minor,
    int w_size, int h_size, int modelization_mode);
*/
int execute_function(int argc, char **argv, SDL_Surface *im, Ui *ui);





#endif
