#include "Open_GL_exec.h"
#include <GL/glut.h> // GLUT, include glu.h and gl.h
#include <GL/freeglut.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cglm/cglm.h>
#include <tesseract/capi.h>
#include <leptonica/allFers.h>

#define PI 3.14159

void VectorsFromAngles();
void RenderString(float x, float y, float z, void *font, const unsigned char* string, float r, float b, float g);

/* Global variables */

// 3D model
static int **bp;
static int **river2;
static int **trail2;
static int **road_major2;
static int **road_minor2;
static _3D_Coord *points_river = NULL;
static _3D_Coord *points_trail = NULL;
static _3D_Coord *points_road_major = NULL;
static _3D_Coord *points_road_minor = NULL;
static int size_river = 10;
static int real_size_river = 0;
static int size_trail = 10;
static int real_size_trail = 0;
static int size_road_major = 10;
static int real_size_road_major = 0;
static int size_road_minor = 10;
static int real_size_road_minor = 0;

static SDL_Surface *image;

static int ntri = 0;
static ITRIANGLE *v;
static XYZ *p = NULL;

// Informations
static _3D_Coord max_point;
static _3D_Coord min_point;
static char *max_altitude = NULL;
static char *min_altitude = NULL;

// Mode
static int mod_mode;
static int add_mode = 0;

//Window
static int window;

//Texture 

static GLuint texture_;
//Camera
static float cam_x;
static float cam_y;
static float mouse_pos_x;
static float mouse_pos_y;
static float height_c;
static float width_c;
static vec3 _position = {0.0f, 0.0f, 0.0f};
static vec3 _target;
static vec3 _forward;
static vec3 _left;
static float _theta = 0;
static float _phi = 0;
static float realspeed = 0.05f;
static float sensivity = 0.05f;

static Attributes att;

//OCR
const char *imgPath = "Pictures/Results/name.bmp"; //Path to the bmp containing our names
static char *ocrTXT; //The text we got with our OCR
static wordCoord *wArr; //The array containing the different mountain names
static size_t arrLen = 0; //The size of wArr
static int canDisp = 1; //The variable that indicates if it's possible to display the names. Set to 0 if the OCR or SDL failed.
static int Twidth = 50;
static int Theight = 20;

void camera();

struct Mo
{
  int forward, backward, left, right;
};

struct Mo motion = {0, 0, 0, 0};


char title[] = "3D Shapes";

/* Initialize OpenGL Graphics */
void initGL(GdkRGBA * arr)
{
    glClearColor(arr->red, arr->green, arr->blue, arr->alpha );              // Set background color to black and opaque
    glClearDepth(1.0f);                                // Set background depth to farthest
    glEnable(GL_DEPTH_TEST);                           // Enable depth testing for z-culling
    glDepthFunc(GL_LEQUAL);                            // Set the type of depth-test
    glShadeModel(GL_SMOOTH);                           // Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Nice perspective corrections
    glutSetCursor(GLUT_CURSOR_NONE); 
    glEnable(GL_TEXTURE_2D);
}

//put the names of the mountains in ocrTXT
void initOCR()
{
  TessBaseAPI *handle;
  PIX *pic;

  if ((pic = pixRead(imgPath)) == NULL)
    {
      printf("Tesseract: Couldn't read image\n");
      canDisp = 0;
      return;
    }

  handle = TessBaseAPICreate();
  if (TessBaseAPIInit3(handle,NULL,"fra") != 0)
    {
      printf("Tesseract: Couldn't initialize Tesseract\n");
      pixDestroy(&pic);
      canDisp = 0;
      return;
    }

  TessBaseAPISetImage2(handle,pic);
  if (TessBaseAPIRecognize(handle,NULL) != 0)
    {
      printf("Tesseract: Couldn't recognize\n");
      pixDestroy(&pic);
      TessBaseAPIEnd(handle);
      TessBaseAPIDelete(handle);
      canDisp = 0;
      return;
    }
  if ((ocrTXT = TessBaseAPIGetUTF8Text(handle)) == NULL)
    {
      printf("Tesseract: Error getting text\n");
      pixDestroy(&pic);
      TessBaseAPIEnd(handle);
      TessBaseAPIDelete(handle);
      canDisp = 0;
      return;
    }

  TessBaseAPIEnd(handle);
  TessBaseAPIDelete(handle);
  pixDestroy(&pic);
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

    glTranslatef(2.0f, -1.0f, -1.0f); // Move right and into the screen
    
    if (mod_mode == 0)
    {    glBindTexture (GL_TEXTURE_2D, texture_);
    	Draw_Triangles(p, v, ntri,bp,image,att);
    	}
    else if(mod_mode == 1)
    	Draw_Points_with_color(bp,image);
    else {
    	Draw_Triangle_Lines(p,v,ntri,bp,image);
    	//Draw_Triangles(p, v, ntri,bp,image);
}   
    if(add_mode == 0 || add_mode == 2)
      Draw_Points_Add(bp, image, 0.0f, 0.0f, 1.0f, points_river, real_size_river);
    if(add_mode == 0 || add_mode == 3)
      Draw_Points_Add(bp, image, 0.58f, 0.30f, 0.0f, points_trail, real_size_trail);
    if(add_mode == 0 || add_mode == 4)
      Draw_Points_Add(bp, image, 1.0f, 1.0f, 0.0f, points_road_major, real_size_road_major);
    if(add_mode == 0 || add_mode == 5)
      Draw_Points_Add(bp, image, 1.0f, 1.0f, 1.0f, points_road_minor, real_size_road_minor);

    int max = biggest_height(image, bp);
    float max_dim_size = biggest_dim_size(image);

    RenderString(((float)min_point.x/max_dim_size) * 2 - image->w / max_dim_size,
        ((float)min_point.y / max_dim_size) * 2 + (image->h / max_dim_size),
        (float)min_point.z / (float)max,
        GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)min_altitude, 0.5f, 0.16f, 0.86f);
    RenderString(((float)max_point.x/max_dim_size) * 2 - image->w / max_dim_size,
        ((float)max_point.y / max_dim_size) * 2 + (image->h / max_dim_size),
        (float)max_point.z / (float)max,
        GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)max_altitude, 0.5f, 0.16f, 0.86f);

    for (size_t i = 0; i < arrLen; i++)
      {
	RenderString(((float)wArr[i].x/max_dim_size) * 2 - image->w / max_dim_size,
		     ((float)wArr[i].y/max_dim_size) * 2 - (image->h / max_dim_size),
		     (float)bp[wArr[i].x][wArr[i].y] / (float)max,
		     GLUT_BITMAP_TIMES_ROMAN_24,
		     (unsigned char*) wArr[i].word,1.0f,1.0f,1.0f);
      }
    //RenderString(0.0f, 0.0f, GLUT_BITMAP_TIMES_ROMAN_24, (unsigned char*)"Hello", 1.0f, 0.0f, 0.0f);

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
        glutDestroyWindow(window);
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
    case GLUT_KEY_LEFT:
        add_mode -= 1;
        if(add_mode < 0)
          add_mode = 5;
        break;
    case GLUT_KEY_RIGHT:
        add_mode += 1;
        if(add_mode > 5)
          add_mode = 0;
        break;
    case GLUT_KEY_DOWN:
        mod_mode -= 1;
        if(mod_mode < 0)
          mod_mode = 2;
        break;
    case GLUT_KEY_UP:
        mod_mode += 1;
        if(mod_mode > 2)
          mod_mode = 0;
        break;
    }
}

void timer(int time)
{
  glutPostRedisplay();
  glutWarpPointer(width_c/2, height_c/2);
  glutTimerFunc(time/60,timer, 0);
}

void RenderString(float x, float y, float z, void *font, const unsigned char* string,
    float r, float b, float g)
{
  glColor3f(r, g, b);
  glRasterPos3f(x, y, z+0.05f);

  glutBitmapString(font, string);
}

void VectorsFromAngles()
{
  vec3 up = {0.0f, 0.0f, 1.0f};
  if(_phi > 89.0f)
    _phi =  89.0f;
  if(_phi < -89.0f)
    _phi = -89.0f;

  float temp = cos(_phi*PI/180);
  _forward[2] = sin(_phi*PI/180); // pitch
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

GLuint LoadTexture( const char * filename )
{
  GLuint texture;
  int width, height;
  unsigned char * data;

  FILE * file;
  file = fopen( filename, "rb" );

  if ( file == NULL ) return 0;
  width = 1024;
  height = 512;
  data = (unsigned char *)malloc( width * height * 3 );
  //int size = fseek(file,);
  fread( data, width * height * 3, 1, file );
  fclose( file );

  for(int i = 0; i < width * height ; ++i)
  {
    int index = i*3;
    unsigned char B,R;
    B = data[index];
    R = data[index+2];

    data[index] = R;
    data[index+2] = B;
  }

  glGenTextures( 1, &texture );
  glBindTexture( GL_TEXTURE_2D, texture );
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );
  
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, 4, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering
  

  free( data );

  return texture;
}


int execute_function(int argc, char **argv, SDL_Surface *im, Ui *ui)
{
glEnable(GL_TEXTURE_2D);
    texture_ = LoadTexture("Pictures/texture.bmp");

    
    GdkRGBA background = {0, 0, 0, 0};

    gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(ui->model.back_color),
            &background);
            

    image = im; //it's to use SDL_Surface *im as a global ref

    mod_mode = gtk_combo_box_get_active(GTK_COMBO_BOX(ui->model.dropdown_type));
    bp = ui->bp;
    river2 = ui->river;
    trail2 = ui->trail;
    road_major2 = ui->road_major;
    road_minor2 = ui->road_minor;
    points_river = NULL;
    points_trail = NULL;
    points_road_major = NULL;
    points_road_minor = NULL;
    size_river = 10;
    real_size_river = 0;
    size_trail = 10;
    real_size_trail = 0;
    size_road_major = 10;
    real_size_road_major = 0;
    size_road_minor = 10;
    real_size_road_minor = 0;
    max_altitude = NULL;
    min_altitude = NULL;


    if (image == NULL)
        printf("SDL_LoadBMP image failed: %s\n", SDL_GetError());

    att = attr(image, bp);
    _3D_Coord *arr = malloc((att.nb_points)* sizeof(_3D_Coord));
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

    points_river = calloc(10, sizeof(_3D_Coord));
    if(points_river == NULL)
    {
      errx(EXIT_FAILURE, "could not malloc points_river");
    }
    points_trail = calloc(10, sizeof(_3D_Coord));
    if(points_trail == NULL)
    {
      errx(EXIT_FAILURE, "could not malloc points_trail");
    }
    points_road_major = calloc(10, sizeof(_3D_Coord));
    if(points_road_major == NULL)
    {
      errx(EXIT_FAILURE, "could not malloc points_road_major");
    }
    points_road_minor = calloc(10, sizeof(_3D_Coord));
    if(points_road_minor == NULL)
    {
      errx(EXIT_FAILURE, "could not malloc points_road_minor");
    }
    min_altitude = calloc(10, sizeof(char));
    if(min_altitude == NULL)
    {
      errx(EXIT_FAILURE, "could not malloc min_altitude");
    }
    max_altitude = calloc(10, sizeof(char));
    if(max_altitude == NULL)
    {
      errx(EXIT_FAILURE, "could not malloc max_altitude");
    }




    real_size_river = Points_To_Lists(bp, river2, image, &points_river, &size_river);
    real_size_trail = Points_To_Lists(bp, trail2, image, &points_trail, &size_trail);
    real_size_road_major = Points_To_Lists(bp, road_major2, image, &points_road_major, &size_road_major);
    real_size_road_minor = Points_To_Lists(bp, road_minor2, image, &points_road_minor, &size_road_minor);

    search_points(&max_point, &min_point, bp, image);

    sprintf(min_altitude, "%d", min_point.z);
    sprintf(max_altitude, "%d", max_point.z);

    initOCR();

    if(SDL_Init(SDL_INIT_VIDEO) == -1)
      canDisp = 0;

    if (canDisp)
      {
	SDL_Surface *ocr = SDL_LoadBMP(imgPath);
    
	if (!ocr)
	  canDisp = 0;
      

	if (canDisp)
	  {
	    char *boolMat = calloc(ocr->h*ocr->w,sizeof(char));

	    for (int i = 0; i < ocr->h; i++)
	      for (int j = 0; j < ocr->w; j++)
		boolMat[i*ocr->w + j] = 1;

	    for (size_t i = 0; ocrTXT[i] != 0; i++)
	      if (ocrTXT[i] == '\n')
		arrLen++;

	    wArr = calloc(arrLen,sizeof(struct wordCoord));

	    size_t begin = 0;

	    size_t num = 0;

	    for (size_t i = 0; ocrTXT[i] != 0; i++)
	      if (ocrTXT[i] == '\n')
		{
		  size_t wSize = i-begin+1;
		  char *nw = calloc(wSize,sizeof(char));

		  for (size_t j = 0; j < wSize; j++)
		    nw[j] = ocrTXT[begin + j];

		  nw[wSize - 1] = 0;

		  wArr[num].word = nw;

		  num++;

		  begin = i+1;
		}

	    size_t wordNum = 0;
	    for (int y = 0; y < ocr->h; y++)
	      for (int x = 0; x < ocr->w; x++)
		{
		  Uint32 pixel = BMP_Get_Pixel(ocr,x,y);
		  Uint8 r,g,b;
		  SDL_GetRGB(pixel, ocr->format, &r, &g, &b);

		  if (r==0 && g == 0 && b == 0 && boolMat[y*ocr->w+x])
		    {
		      wArr[wordNum].x = (int)x;
		      wArr[wordNum].y = (int)y;

		      wordNum++;
		  
		      for (int yy = 0; yy <= Theight && y+yy < ocr->h; yy++)
			{
			  for (int xx = 0; xx <= Twidth && x-xx >= 0; xx++)
			    {
			      boolMat[(y+yy)*ocr->w + x-xx] = 0;
			    }

			  for (int xx = 1;xx <= Twidth && x+xx < ocr->w;xx++)
			    {
			      boolMat[(y+yy)*ocr->w + x+xx] = 0;
			    }
			}
		    }
		}

	    free(boolMat);
	  }

	SDL_FreeSurface(ocr);
      }

    /*printf("%i\n", points_river[0]);
    printf("%i\n", points_river[1]);
    printf("%i\n", points_river[2]);*/


    //printf("add %i\n", points_river);
    //Print_Arr_of_Coord(att.nb_points, x);

    glutInit(&argc, argv);                // Initialize GLUT
    glutInitDisplayMode(GLUT_RGB |GLUT_DOUBLE| GLUT_DEPTH);     // Enable double buffered mode
    glutInitWindowSize(ui->model.width, ui->model.height);         // Set the window's initial width & height
    glutInitWindowPosition(500, 50);      // Position the window's initial top-left corner
    glutCreateWindow(title); // Create window with the given title
    window = glutGetWindow();
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    glutDisplayFunc(display);             // Register callback handler for window re-paint event
    glutReshapeFunc(reshape);
    glutIdleFunc(display); // Register callback handler for window re-size event
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboard_up);
    glutSpecialFunc(SpecialKeys);
    glutPassiveMotionFunc(mouse_handler);
    glutTimerFunc(0, timer, 0);
    initGL(&background);       // Our own OpenGL initialization 
    glutSetCursor(GLUT_CURSOR_NONE);

    glutMainLoop(); // Enter the infinite event-processing loop
    //glutDestroyWindow(window);
    //free_bm(bp, image);
    /*free(points_river);
    free(points_trail);
    free(points_road_major);
    free(points_road_minor);*/
    return 0;
}
