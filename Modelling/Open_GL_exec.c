#include "Open_GL_exec.h"
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <stdio.h>
#include <math.h>

/* Global variables */
static int **bp;

static SDL_Surface *image;

static float angle_Pitch = 0.0;

static float angle_Yaw = 0.0;

static float lx = 0.0f, lz = -1.0f, ly = 0;

static float x = 0.0f, z = 5.0f;

static float y = 1.0f;

char title[] = "3D Shapes";

/* Initialize OpenGL Graphics */
void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);              // Set background color to black and opaque
    glClearDepth(1.0f);                                // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);                           // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);                            // Set the type of depth-test
    glShadeModel(GL_SMOOTH);                           // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Nice perspective corrections
}

/* Handler for window-repaint event. Called back when the window is created, displaced or resized. 
or when camera is displaced */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glMatrixMode(GL_MODELVIEW);                         // To operate on model-view matrix
    glLoadIdentity();                                   // Reset the model-view matrix
    gluLookAt(x, y, z,                                  //camera set-up
              x + lx, y + ly, z + lz,
              0.0f, 1.0f, 0.0f);
    glTranslatef(1.5f, 0.0f, -7.0f); // Move right and into the screen

    //Draw_Points(bp,image);
    glBegin(GL_POINTS);
    glColor3f(0.0f, 1.0f, 0.5f);
    for (int i = 0; i < (image->w); i++)
    {
        for (int j = 0; j < (image->h); j++)
        {
	//	printf("%d \n",bp[i][j]);
            if (bp[i][j] != 0)
            {
                glVertex3f(((float)j / (float)image->w), ((float)i / (float)image->h),
                           ((float) bp[i][j]/1500.));
            }
        }
    }
    glEnd();

    glutSwapBuffers(); // Swap the front and back frame buffers (double buffering)

    /* To render Pyramid uncomment
   // Render a pyramid consists of 4 triangles
   glBegin();   // begin drawing pyramid
   glLoadIdentity();                  // Reset the model-view matrix
   glTranslatef(-1.5f, 0.0f, -6.0f);  // Move left and into the screen

   glBegin(GL_TRIANGLES);           // Begin drawing the pyramid with 4 triangles
      // Front
      glColor3f(1.0f, 0.0f, 0.0f);     // Red
      glVertex3f( 0.0f, 1.0f, 0.0f);
      glColor3f(0.0f, 1.0f, 0.0f);     // Green
      glVertex3f(-1.0f, -1.0f, 1.0f);
      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
      glVertex3f(1.0f, -1.0f, 1.0f);
 
      // Right
      glColor3f(1.0f, 0.0f, 0.0f);     // Red
      glVertex3f(0.0f, 1.0f, 0.0f);
      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
      glVertex3f(1.0f, -1.0f, 1.0f);
      glColor3f(0.0f, 1.0f, 0.0f);     // Green
      glVertex3f(1.0f, -1.0f, -1.0f);
      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
      glVertex3f(-1.0f, -1.0f, -1.0f);
      glVertex3f( 0.0f, 1.0f, 0.0f);
      glColor3f(0.0f,0.0f,1.0f);       // Blue
      glVertex3f(-1.0f,-1.0f,-1.0f);
      glColor3f(0.0f,1.0f,0.0f);       // Green
      glVertex3f(-1.0f,-1.0f, 1.0f);
   glEnd();   // Done drawing the pyramid
 
   glutSwapBuffers();  // Swap the front and back frame buffers (double buffering)
}
 */
}
/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height)
{ // GLsizei for non-negative integer
    // Compute aspect ratio of the new window
    if (height == 0)
        height = 1; // To prevent divide by 0
    GLfloat aspect = (GLfloat)width / (GLfloat)height;

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);

    // Set the aspect ratio of the clipping volume to match the viewport
    glMatrixMode(GL_PROJECTION); // To operate on the Projection matrix
    glLoadIdentity();            // Reset
    // Enable perspective projection with fovy, aspect, zNear and zFar
    gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void keyboard(unsigned char key) //camera translation
{
    switch (key)
    {
    case 'w': //
        y += .05f;
        break;
    case 's':
        y -= .05f;
        break;
    case 'a':
        x -= .05f;
        break;
    case 'd':
        x += .05f;
        break;
    case 'i':
        z -= .05f;
        break;
    case 'k':
        z += .05f;
        break;
    case 27:
        exit(0);
    }
}

void SpecialKeys(int key) //camera rotation
{
    //float fraction = 0.1f; //rotate speed
    switch (key)
    {
    case GLUT_KEY_LEFT:
        angle_Yaw -= 0.01f;
        lx = sin(angle_Yaw);
        lz = -cos(angle_Yaw);
        break;
    case GLUT_KEY_RIGHT:
        angle_Yaw += 0.01f;
        lx = sin(angle_Yaw);
        lz = -cos(angle_Yaw);
        break;
    case GLUT_KEY_UP:

        angle_Pitch += 0.01f;
        ly = tan(angle_Pitch);
        break;
    case GLUT_KEY_DOWN:
        angle_Pitch -= 0.01f;
        ly = tan(angle_Pitch);
        break;
    }
}

int execute_function(int argc, char **argv, SDL_Surface *im, int** bps)
{
	image = im; //it's to use SDL_Surface *im as a global ref
        bp = bps;
    /* Pierre-Corentin stuff
        float c = eye[2];
        radius = c; */

    if (image == NULL)
        printf("SDL_LoadBMP image failed: %s\n", SDL_GetError());
    /*
    bp = NULL;
    bp = (int **)calloc(image->w, sizeof(int *));
    for (int k = 0; k < image->w; k++)
    {
        bp[k] = (int *)calloc(image->h, sizeof(int));
    }
    Map_Colorisation(image, bp);*/

    glutInit(&argc, argv);             // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE); // Enable double buffered mode
    glutInitWindowSize(640, 480);     // Set the window's initial width & height
    glutInitWindowPosition(500, 50);  // Position the window's initial top-left corner
    glutCreateWindow(title);          // Create window with the given title
    glutDisplayFunc(display);         // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);
    glutIdleFunc(display); // Register callback handler for window re-size event
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(SpecialKeys);
    initGL();       // Our own OpenGL initialization
    
    glutMainLoop(); // Enter the infinite event-processing loop
    free_bm(bp, image);
    return 0;
}
