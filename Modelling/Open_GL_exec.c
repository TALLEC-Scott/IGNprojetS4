#include "Open_GL_exec.h"
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <GL/freeglut.h>
#include <stdio.h>
#include <math.h>
#include <cglm/cglm.h>

#define RADIAN 3.14/180.0
#define degreesToRadians(x) x*(3.141592f/180.0f)

void rotateX(float angle);
void rotateY(float angle);
void GetViewMatrix(mat4 res);
void updateCameraVectors();


/* Global variables */
static int **bp;
static int **river2;

static SDL_Surface *image;

static float angle_Pitch = 0.0;

static float angle_Yaw = -90.0f;

//static float angle_Roll = 0.0;

static float lx = 0.0f, lz = -1.0f, ly = 0;

static float x = 0.0f, z = 5.0f;

static float y = 1.0f;

static float angle = 0.0f;

static float ang = 0.0f;

static float deltaAngle = 0.0f;

static float gammaAngle = 0.0f;

static int xOrigin = -1;

static int yOrigin = -1;

static int ntri = 0;
static ITRIANGLE *v;
static XYZ *p = NULL;

//Camera
static float cam_x;
static float cam_y;
static float old_cam_y = 0.0f, old_cam_x = 0.0f;
static float mouse_pos_x;
static float mouse_pos_y;
static float height_c;
static float width_c;
static float camX = 0.0f;
static float camZ = 0.0f;
static float theta = 0.0f;
static float phi = 0.0f;
static vec3 (forward) = {0.0f, 0.0f, 0.0f};
static vec3 (up) = {0.0f, 1.0f, 0.0f};
static vec3 (left) = {1.0f, 1.0f, 1.0f};
static vec3 (position) = {0.0f, 0.0f, 0.0f};
static vec3 (orientation) = {1.0f, 1.0f, 1.0f};
static float fov = 45.0f;
static int firstMouse = 1;



// Step translation
static float max_step = 0.125f;
static float min_step = 0.0625f;
static float current_step = 0.125f;

// Step rotation
static float max_step_rot = 0.02f;
static float min_step_rot = 0.01f;
static float current_step_rot = 0.01f;


// QUATERNION
static vec3 Position = {0.0f, 1.0f, 0.0f};
static versor Orientation = {0, 0, 0, -1};

enum camera_movement{FORWARD, BACKWARD, LEFT, RIGHT};
const float SPEED = 10.0f;
const float SENSITIVITY = 0.01f;
const float ZOOM = 45.0f;
float RightAngle = 0.0f;
float UpAngle = 0.0f;

float MovementSpeed;
float MouseSensitivity;
float Zoom;

void camera();

struct Mo
{
  int forward, backward, left, right;
};

struct Mo motion = {0, 0, 0, 0};


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


    


    /*gluLookAt(position[0], position[1], position[2],
        position[0]+forward[0],
        position[1]+forward[1],
        position[2]+forward[2],
        up[0], up[1], up[2]);*/
    mat4 M;

    GetViewMatrix(M);
    //printf("%f, %f, %f\n", M[3][0], M[3][1], M[3][2]);


    glMultMatrixf(*M);

    glTranslatef(0.0f, 0.0f, 0.0f); // Move right and into the screen

    Draw_Points(river2,image);
    Draw_Triangles(p, v, ntri,bp,image);
    
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
    gluPerspective(fov, aspect, 0.1f, 100.0f);
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
    case 'k':
        z -= current_step;
        glutPostRedisplay();
        break;
    case 'i':
        z += current_step;
        glutPostRedisplay();
        break;
    /*case 73:
        z += current_step;
        break;*/
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
    //float fraction = 0.1f; //rotate speed
    switch (key)
    {
    case GLUT_KEY_LEFT:
        angle_Yaw -= current_step_rot;
        lx = sin(angle_Yaw);
        lz = -cos(angle_Yaw);
        break;
    case GLUT_KEY_RIGHT:
        angle_Yaw += current_step_rot;
        lx = sin(angle_Yaw);
        lz = -cos(angle_Yaw);
        break;
    case GLUT_KEY_UP:
        angle_Pitch += current_step_rot;
        ly = tan(angle_Pitch);
        break;
    case GLUT_KEY_DOWN:
        angle_Pitch -= current_step_rot;
        ly = tan(angle_Pitch);
        break;
    case GLUT_KEY_SHIFT_L:
        current_step = max_step;
        current_step_rot = max_step_rot;
        break;
    case GLUT_KEY_CTRL_L:
        current_step = min_step;
        current_step_rot = min_step_rot;
        break;
    }
}

void mouseButton(int button, int state, int xx, int yy) {

    // only start motion if the left button is pressed
    if (button == GLUT_LEFT_BUTTON)
    {

        // when the button is released
        if (state == GLUT_UP) {
            ang+=gammaAngle;
            yOrigin = -1;
        }
        else  {// state = GLUT_DOWN
            yOrigin = yy;
        }
    }

    if (button == GLUT_RIGHT_BUTTON) {

        // when the button is released
        if (state == GLUT_UP) {
            angle += deltaAngle;
            xOrigin = -1;
        }
        else  {// state = GLUT_DOWN
            xOrigin = xx;
        }
    }
}

void GetViewMatrix(mat4 res)
{
  versor reverseOrient;
  glm_quat_conjugate(Orientation, reverseOrient);
  mat4 rot;
  glm_quat_mat4(Orientation, rot);

  rot[3][0] = -(rot[0][0] * Position[0] + rot[1][0] * Position[1] + rot[2][0] * Position[3]);
  rot[3][1] = -(rot[0][1] * Position[0] + rot[1][1] * Position[1] + rot[2][1] * Position[3]);
  rot[3][2] = -(rot[0][2] * Position[0] + rot[1][2] * Position[1] + rot[2][2] * Position[3]);
  rot[3][3] = 1;

  glm_mat4_copy(rot, res);


  /*mat4 translation;
  //

  glm_mat4_mul(rot, translation, res);*/
}

void timer(int time)
{
  glutPostRedisplay();
  glutWarpPointer(width_c/2, height_c/2);
  glutTimerFunc(time/60,timer, 0);
}

void mouse_handler(int x, int y)
{

  cam_x = ((width_c/2) - x)*0.05f;
  cam_y = ((height_c / 2) - y)*0.05f;

  RightAngle += cam_x;
  UpAngle += cam_y;

  updateCameraVectors();


  //rotateX(cam_x);
  //rotateY(cam_y);

  /*angle_Yaw += cam_x;
  angle_Pitch -= cam_y;

  if(angle_Pitch > 89.0f)
    angle_Pitch =  89.0f;
  if(angle_Pitch < -89.0f)
    angle_Pitch = -89.0f;

  vec3 direction;
  direction[0] = cos(glm_rad(angle_Yaw)) * cos(glm_rad(angle_Pitch));
  direction[1] = sin(glm_rad(angle_Pitch));
  direction[2] = sin(glm_rad(angle_Yaw)) * cos(glm_rad(angle_Pitch));
  glm_vec3_normalize_to(direction, forward);*/

  //printf("%f, %f\n", cam_x, cam_y);
}

void updateCameraVectors()
{
  versor aroundY;
  versor aroundX;
  vec3 v1 = {0, 1, 0};
  vec3 v2 = {1, 0, 0};
  glm_quatv(aroundY, glm_rad(-RightAngle), v1);
  glm_quatv(aroundX, glm_rad(UpAngle), v2);

  glm_quat_mul(aroundX, aroundY, Orientation);
}


void rotateY(float angle)
{
  double radians = glm_rad(angle);
  vec3 v1;
  vec3 v2;
  glm_vec3_scale(left, (float)sin(radians), v1);
  glm_vec3_scale(forward, (float)cos(radians), v2);
  vec3 v3;
  glm_vec3_add(v1, v2, v3);
  glm_vec3_normalize_to(v3, forward);
  vec3 dest;
  glm_vec3_cross(up, forward, dest);
  glm_vec3_normalize_to(dest, left);
}

/*void rotate(float angleX, float angleY)
{
  float newX = fmod((position[0] + angleX),360.0f);
  float newY = fmod((position[1] + angleY),360.0f);

  printf("%f, %f, %f\n", position[0], position[1], position[2]);

  position[0] = newX;
  position[1] = newY;
}*/


void camera()
{
  versor q1 = {0, 0, 0, -1};
  versor temp;
  glm_quat_conjugate(Orientation, temp);
  versor temp2;
  versor qF;
  glm_quat_mul(Orientation, q1, temp2);
  glm_quat_mul(temp2, temp, qF);
  vec3 Front = {qF[0], qF[1], qF[2]};
  vec3 temp3;
  vec3 v1 = {0, 1, 0};
  glm_vec3_cross(Front, v1, temp3);
  vec3 Right;
  glm_vec3_normalize_to(temp3, Right);
  




  if(motion.forward == 1)
  {
    vec3 forw;
    glm_vec3_scale(Front, 0.05f, forw);
    glm_vec3_add(Position, forw, Position);
  }
  if(motion.backward == 1)
  {
    vec3 back;
    Front[0] = -Front[0];
    Front[1] = -Front[1];
    Front[2] = -Front[2];
    glm_vec3_scale(Front, 0.05f, back);
    glm_vec3_add(Position, back, Position);
    printf("%f\n", Front[1]);
  }
  if(motion.left == 1)
  {
    vec3 left;
    Right[0] = -Right[0];
    Right[1] = -Right[1];
    Right[2] = -Right[2];
    glm_vec3_scale(Right, 0.05f, left);
    glm_vec3_add(Position, left, Position);
  }
  if(motion.right == 1)
  {
    vec3 righ;
    glm_vec3_scale(Right, 0.05f, righ);
    glm_vec3_add(Position, righ, Position);
  }

  /*if(angle_Pitch >= 70)
    angle_Pitch = 70;
  if(angle_Pitch <= -60)
    angle_Pitch = -60;

  //glRotatef(-angle_Pitch, 1.0, 0.0, 0.0);
  //glRotatef(-angle_Yaw, 0.0, 1.0, 0.0);
  glTranslatef(-camX, 0.0, -camZ);*/
}

/*void mouseMove(int xx __attribute__((unused)), int yy) {

    // this will only be true when the left button is down
    if (yOrigin >= 0) {

        // update gammaAngle

        gammaAngle = (yy - yOrigin) * 0.01f;

        lz = -cos(ang+gammaAngle);
        ly = sin(ang+gammaAngle);

        glutPostRedisplay();
    }

    if (xOrigin >= 0) {

        // update deltaAngle
        deltaAngle = (xx - xOrigin) * 0.001f;
        // update camera's direction
        lx = -cos(angle+deltaAngle);
        ly = sin(angle+deltaAngle);
        glutPostRedisplay();
    }
}*/

int execute_function(int argc, char **argv, SDL_Surface *im, int **bps,
    int **river)
{

    image = im; //it's to use SDL_Surface *im as a global ref
    bp = bps;
    river2 = river;

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
    //Draw_Points(bp, im);

  
    //Print_Arr_of_Coord(att.nb_points, x);

    glutInit(&argc, argv);                // Initialize GLUT
    glutInitDisplayMode(GLUT_DOUBLE);     // Enable double buffered mode
    glutInitWindowSize(640, 480);         // Set the window's initial width & height
    glutInitWindowPosition(500, 50);      // Position the window's initial top-left corner
    glutCreateWindow(title); // Create window with the given title
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    glutDisplayFunc(display);             // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);
    glutIdleFunc(display); // Register callback handler for window re-size event
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);
    glutPassiveMotionFunc(mouse_handler);
    glutTimerFunc(0, timer, 0);

    glutMouseFunc(mouseButton);
    //glutMotionFunc(mouseMove);

    glutSpecialFunc(SpecialKeys);
    initGL();       // Our own OpenGL initialization
    
    glutMainLoop(); // Enter the infinite event-processing loop
    //glutDestroyWindow(window);
    //free_bm(bp, image);
    return 0;
}
