#include "Open_GL_exec.h"
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <cglm/cglm.h>

#define PI 3.14159

void VectorsFromAngles();

/* Global variables */
static int **bp;
static int **river2;
static int **trail2;
static int **road_major2;
static int **road_minor2;

static SDL_Surface *image;

static int ntri = 0;
static ITRIANGLE *v;
static XYZ *p = NULL;

static int mod_mode

//Camera
static float cam_x;
static float cam_y;
static float mouse_pos_x;
static float mouse_pos_y;
static float height_c;
static float width_c;
static vec3 _position = {0.0f, -0.5f, 1.5f};
static vec3 _target;
static vec3 _forward;
static vec3 _left;
static float _theta = 0;
static float _phi = 0;
static float realspeed = 0.05f;
static float sensivity = 0.05f;
static int first = 1;
void camera();

struct Mo
{
  int forward, backward, left, right;
};

struct Mo motion = {0, 0, 0, 0};


char title[] = "3D Shapes";

/* Initialize OpenGL Graphics */
void initGL(float * arr)
{
    glClearColor(arr[0], arr[1], arr[2], 1.0f);              // Set background color to black and opaque
    glClearDepth(1.0f);                                // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);                           // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);                            // Set the type of depth-test
    glShadeModel(GL_SMOOTH);                           // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Nice perspective corrections
    glutSetCursor(GLUT_CURSOR_NONE); 
}

/* Handler for window-repaint event. Called back when the window is created, displaced or resized. 
or when camera is displaced */
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);                         // To operate on model-view matrix
    glLoadIdentity();                                   // Reset the model-view matrix
    camera();
    //printf("%f, %f, %f\n", _target[0], _target[1], _target[2]);

    gluLookAt(_position[0], _position[1], _position[2],
        _target[0],
        _target[1],
        _target[2],
        0, 0, 1);

    //glTranslatef(0.0f, 0.0f, 0.0f); // Move right and into the screen

    
    if (
    Draw_Triangles(p, v, ntri,bp,image);

    Draw_Points_Add(bp, river2, image, 0.0f, 0.0f, 1.0f);
    Draw_Points_Add(bp, trail2, image, 0.58f, 0.30f, 0.0f);
    //Draw_Points_Add(bp, road_major2, image, 1.0f, 1.0f, 0.0f);
   // Draw_Points_Add(bp, road_minor2, image, 1.0f, 1.0f, 1.0f);

    glPopMatrix();
    glutSwapBuffers(); // Swap the front and back frame buffers (double buffering)

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
    height_c = height;
    width_c = width;
    mouse_pos_x = width / 2;
    mouse_pos_y = height / 2;
    glutWarpPointer(mouse_pos_x, mouse_pos_y);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int a __attribute__((unused)), int b __attribute__((unused))) //camera translation
{
    switch (key)
    {
    case 'w': //
        motion.forward = 1;
        glutPostRedisplay();
        break;
    case 's':
        motion.backward = 1;
        glutPostRedisplay();
        break;
    case 'a':
        motion.left = 1;
        glutPostRedisplay();
        break;
    case 'd':
        motion.right = 1;
        glutPostRedisplay();
        break;
    case GLUT_KEY_SHIFT_L:
        realspeed = 0.05f;
        break;
    case 27:
        glutLeaveMainLoop();
    }
}

void keyboard_up(unsigned char key, int a __attribute__((unused)), int b __attribute__((unused)))
{
    switch (key)
    {
    case 'w': //
        motion.forward = 0;
        break;
    case 's':
        motion.backward = 0;
        break;
    case 'a':
        motion.left = 0;
        break;
    case 'd':
        motion.right = 0;
        break;
    case 27:
        glutLeaveMainLoop();
    }
}

void SpecialKeys(int key, int a __attribute__((unused)), int b __attribute__((unused))) //camera rotation
{
    switch (key)
    {
    case GLUT_KEY_SHIFT_L:
        realspeed = 0.05f;
        break;
    case GLUT_KEY_CTRL_L:
        realspeed = 0.02f;
        break;

    }
}

void timer(int time)
{
  glutPostRedisplay();
  glutWarpPointer(width_c/2, height_c/2);
  glutTimerFunc(time/60,timer, 0);
}

void VectorsFromAngles()
{
  vec3 up = {0.0f, 0.0f, 1.0f};
  if(_phi > 89.0f)
    _phi =  89.0f;
  if(_phi < -89.0f)
    _phi = -89.0f;

  float temp = cos(_phi*PI/180);
  _forward[2] = sin(_phi*PI/180);
  _forward[0] = temp*cos(_theta*PI/180);
  _forward[1] = temp*sin(_theta*PI/180);

  glm_vec3_cross(up, _forward, _left);
  glm_vec3_normalize(_left);

  glm_vec3_add(_position, _forward, _target);
}

void mouse_handler(int x, int y)
{
  cam_x = ((width_c/2) - x)*sensivity;
  cam_y = ((height_c / 2) - y)*sensivity;

  _theta += cam_x;
  _phi += cam_y;


  VectorsFromAngles();
}

void camera()
{
  if(motion.forward == 1)
  {
    vec3 temp;
    glm_vec3_scale(_forward, realspeed, temp);
    glm_vec3_add(_position, temp, _position);
  }
  if(motion.backward == 1)
  {
    vec3 temp;
    glm_vec3_scale(_forward, realspeed, temp);
    temp[0] = -temp[0];
    temp[1] = -temp[1];
    temp[2] = -temp[2];
    glm_vec3_add(_position, temp, _position);
  }
  if(motion.left == 1)
  {
    vec3 temp;
    glm_vec3_scale(_left, realspeed, temp);
    glm_vec3_add(_position, temp, _position);
  }
  if(motion.right == 1)
  {
    vec3 temp;
    glm_vec3_scale(_left, realspeed, temp);
    temp[0] = -temp[0];
    temp[1] = -temp[1];
    temp[2] = -temp[2];
    glm_vec3_add(_position, temp, _position);
  }

  glm_vec3_add(_position, _forward, _target);
}



int execute_function(int argc, char **argv, SDL_Surface *im, int **bps,
    int **river, int **trail, int **road_major, int **road_minor,
    int w_size, int h_size, int modelization_mode, float * RGB_Background)
{

    image = im; //it's to use SDL_Surface *im as a global ref
    bp = ui->bps;
    river2 = ui->river;
    trail2 = ui->trail;
    road_major2 = ui->road_major;
    road_minor2 = ui->road_minor;

    if (image == NULL)
        printf("SDL_LoadBMP image failed: %s\n", SDL_GetError());

    Attributes att = attr(image, bp);
    _3D_Coord *arr = malloc(att.nb_points * sizeof(_3D_Coord));
    List_of_Points(bp, image, &arr);

    int nb_points = att.nb_points;

    int nv = 0;

    for (int i = 0; i < nb_points; i++)
    {
        p = realloc(p, (nv + 1) * sizeof(XYZ));
        p[nv].x = arr[i].x;
        p[nv].y = arr[i].y;
        p[nv].z = arr[i].z;
        nv++;
    }

    p = realloc(p, (nv + 3) * sizeof(XYZ));
    v = malloc(3 * nv * sizeof(ITRIANGLE));
    qsort(p, nv, sizeof(XYZ), XYZCompare); 
    Triangulate(nv, p, v, &ntri);

  
    //Print_Arr_of_Coord(att.nb_points, x);

    glutInit(&argc, argv);                // Initialize GLUT
    glutInitDisplayMode(GLUT_RGB |GLUT_DOUBLE| GLUT_DEPTH);     // Enable double buffered mode
    glutInitWindowSize(w_size, h_size);         // Set the window's initial width & height
    glutInitWindowPosition(500, 50);      // Position the window's initial top-left corner
    glutCreateWindow(title); // Create window with the given title
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    glutDisplayFunc(display);             // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);
    glutIdleFunc(display); // Register callback handler for window re-size event
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);
    glutSpecialFunc(SpecialKeys);
    glutPassiveMotionFunc(mouse_handler);
    glutTimerFunc(0, timer, 0);
    initGL();       // Our own OpenGL initialization 


    glutMainLoop(RGB_Background); // Enter the infinite event-processing loop
    //glutDestroyWindow(window);
    //free_bm(bp, image);
    return 0;
}
