#ifndef OPEN_GL_EXEC_H
#define OPEN_GL_EXEC_H
#include "drawing_tools.h"
#include "MapColorisation.h"


void initGL();
void display();
void reshape(GLsizei width, GLsizei height);
void keyboard(unsigned char key);
void SpecialKeys(int key);
int execute_function(int argc, char** argv,SDL_Surface *im, int** bps);






#endif
