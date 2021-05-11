#include "drawing_tools.h"
#define WORD unsigned int

/* Extrapolates the size biggest of the dim(height or width), so that when we "norm" we don't get numbers >1 and at the same point
don't lose the x to y proportion */
float biggest_dim_size(SDL_Surface *image)
{
    if (image->w > image->h)
    {
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
int biggest_height(SDL_Surface *image, int **bp)
{
    int max = 0;
    for (int i = 0; i < image->w; i++)
    {
        for (int j = 0; j < image->h; j++)
        {
            if (bp[i][j] > max)
                max = bp[i][j];
        }
    }
    return max;
}

int first_height(SDL_Surface *image, int **bp)
{
    for (int i = 0; i < image->w; i++)
    {
        for (int j = 0; j < image->h; j++)
        {
            if (bp[i][j] != -1)
                return bp[i][j];
        }
    }
    return 0;
}

Attributes attr(SDL_Surface *image, int **bp)
{
    struct Attributes x;
    int max, min = first_height(image, bp);
    int nb_points = 0;
    for (int i = 0; i < image->w; i++)
    {
        for (int j = 0; j < image->h; j++)
        {
            if (bp[i][j] != -1)
            {
                nb_points++;
                if (bp[i][j] > max)
                    max = bp[i][j];
                if (bp[i][j] < min)
                    min = bp[i][j];
            }
        }
    }
    x.greatest = max;
    x.smallest = min;
    x.nb_points = nb_points;
    return x;
}
/*
struct Node
{
        int x;
        int y;
        struct Node *next;
};

void append(struct Node** head_ref, int new_data_x, int new_data_y)
{
    // 1. allocate node 
    struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
  
    struct Node *last = *head_ref;  // used in step 5
   
    // 2. put in the data  
    new_node->x  = new_data_x;
    new_node->y = new_data_y;
  
    // 3. This new node is going to be the last node, so make next 
     //     of it as NULL
    new_node->next = NULL;
  
    // 4. If the Linked List is empty, then make the new node as head 
    if (*head_ref == NULL)
    {
       *head_ref = new_node;
       return;
    }  
       
    //5. Else traverse till the last node 
    while (last->next != NULL)
        last = last->next;
   
    // 6. Change the next of last node 
    last->next = new_node;
    return;    
}

struct node *start=NULL;

int** All_heights(SDL_Surface *image, int **bp){
    struct Greatest_Smallest x = range(image,bp);
    int range = (x.greatest -x.smallest)/100;
    int map[range];

    int *indices = calloc(range,sizeof(int));
    int **map = calloc(range,sizeof(int));
    for (int i = 0; i < range; i++)
    {
        map[i] = calloc(indices[i],sizeof(int)*2)
    }

    for (int i = 0; i < image->w; i++)
    {
        for (int j = 0; j < image->h; j++){
            if (bp[i][j]!= 0){
                append(&(map[bp[i][j]-x.smallest))/100],i,j);
            }
        }
    }
}

float dist()(int x, int y, int a, int b){
    return sqrt((y-b)**2 + (x-a)**2);
}

int* Closest_point(int **map,int i, int j,int alt_index, int** markage){ //don't call for point of max altitude
    int res[2];
    float max = 0.0f;
    stuct Node *points = map[alt_index+1];
    struct Node *last = *head_ref;
    while (last->next != NULL){
        if markage[last->x][last->y] = 0;{
        float dist = dist(i,j,last->x,last->y);
        if(dist> max){
            max = dist;
            res[0] = last->x;
            res[1] = last->y;

        last = last->next;
        }}
    markage[res[0]][res[1]] = 1;
    return res;
}


void Draw_Top_Line(int **map, int range, SDL_Surface *image){
    int max = biggest_height(image, bp);
    struct Node *last = &map[range-1];
    while (last->next != NULL){
        glBegin(GL_POINTS);
        glColor3f(0.0f, 1.0f, 0.5f);
        glVertex3f(((float)last->x / max_dim_size)*2 -image->w/max_dim_size, ((float)last->y / max_dim_size)*2 +(image->h/max_dim_size),
                           (float) max;
    }
    glEnd();


}

    */
/* Draws Points from 2D array M (currently lists of list) */

void List_of_Points(int **bp, SDL_Surface *image, _3D_Coord **res)
{
    _3D_Coord *_res = *res;
    int index = 0;
    _3D_Coord co;
    for (int i = 0; i < image->w; i++)
    {
        for (int j = 0; j < image->h; j++)
        {

            if (bp[i][j] != -1)
            {
                co.x = i;
                co.y = j;
                co.z = bp[i][j];
                _res[index] = co;
                index++;
            }
        }
    }
}

void Print_Arr_of_Coord(int nb_points, _3D_Coord arr[])
{
    for (int i = 0; i < nb_points; i++)
    {
        printf("x: %d", arr[i].x);
        printf(" y: %d", arr[i].y);
        printf(" z: %d \n", arr[i].z);
    }
}

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
            if (bp[i][j] != -1)
            {
                glVertex3f(((float)i / max_dim_size) * 2 - image->w / max_dim_size, ((float)j / max_dim_size) * 2 + (image->h / max_dim_size),
                           (float)bp[i][j] / (float)max);
            }
        }
    }

    glEnd();
}

void Get_Triangles(int **bp, SDL_Surface *image, _3D_Coord arr[])
{

    Attributes att = attr(image, bp);
    int nb_points = att.nb_points;
    
    int ntri = 0;
    ITRIANGLE *v;
    XYZ *p = NULL;
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
}

void Draw_Triangles(XYZ *p, ITRIANGLE *v, int ntri, int **bp, SDL_Surface *image )

{
    int max = biggest_height(image, bp);
    float max_dim_size = biggest_dim_size(image);

    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 1.0f, 0.5f);
    for (int i = 0; i < ntri; i++)
    {
        if( ((float)p[v[i].p1].z == 1500) | ((float)p[v[i].p2].z == 1500) | ((float)p[v[i].p3].z == 1500))
      {
         glColor3f(128/255., 0/255., 0/255.);
      }
      else if( ((float)p[v[i].p1].z == 1400) | ((float)p[v[i].p2].z == 1400) | ((float)p[v[i].p3].z == 1400))
      {
         glColor3f( 255/255., 0/255., 0/255.);
      }
      else if( ((float)p[v[i].p1].z == 1300) | ((float)p[v[i].p2].z == 1300) | ((float)p[v[i].p3].z == 1300))
      {
         glColor3f( 255/255., 127/255., 0/255.);
      }
      else if( ((float)p[v[i].p1].z == 1200) | ((float)p[v[i].p2].z == 1200) | ((float)p[v[i].p3].z == 1200))
      {
         glColor3f( 255/255., 102/255., 0/255.);
      }
      else if( ((float)p[v[i].p1].z == 1100) | ((float)p[v[i].p2].z == 1100) | ((float)p[v[i].p3].z == 1100))
      {
         glColor3f( 255/255., 153/255., 51/255.);
      }
      else if( ((float)p[v[i].p1].z == 1000) | ((float)p[v[i].p2].z == 1000) | ((float)p[v[i].p3].z == 1000))
      {
         glColor3f( 255/255., 204/255., 102/255.);
      }
      else if( ((float)p[v[i].p1].z == 900) | ((float)p[v[i].p2].z == 900) | ((float)p[v[i].p3].z == 900))
      {
         glColor3f( 255/255., 255/255, 153/255.);
      }
      else if( ((float)p[v[i].p1].z == 800) | ((float)p[v[i].p2].z == 800) | ((float)p[v[i].p3].z == 800))
      {
         glColor3f( 204/255., 255/255., 153/255.);
      }
      else if( ((float)p[v[i].p1].z == 700) | ((float)p[v[i].p2].z == 700) | ((float)p[v[i].p3].z == 700))
      {
         glColor3f( 153/255., 255/255., 153/255.);
      }
      else if( ((float)p[v[i].p1].z == 600) | ((float)p[v[i].p2].z == 600) | ((float)p[v[i].p3].z == 600))
      {
         glColor3f( 102/255., 255/255., 153/255.);
      }
      else if( ((float)p[v[i].p1].z == 500) | ((float)p[v[i].p2].z == 500) | ((float)p[v[i].p3].z == 500))
      {
         glColor3f( 0/255., 255/255., 153/255.);
      }
      else if( ((float)p[v[i].p1].z == 400) | ((float)p[v[i].p2].z == 400) | ((float)p[v[i].p3].z == 400))
      {
         glColor3f( 0/255., 255/255., 255/255.);
      }
      else if( ((float)p[v[i].p1].z == 300) | ((float)p[v[i].p2].z == 300) | ((float)p[v[i].p3].z == 300))
      {
         glColor3f( 0/255., 204/255., 255/255.);
      }
      else if( ((float)p[v[i].p1].z == 200) | ((float)p[v[i].p2].z == 200) | ((float)p[v[i].p3].z == 200))
      {
         glColor3f( 0/255., 102/255., 204/255.);
      }
      else if( ((float)p[v[i].p1].z == 100) | ((float)p[v[i].p2].z == 100) | ((float)p[v[i].p3].z == 100))
      {
         glColor3f( 0/255., 51/255., 204/255.);
      }
      else if( ((float)p[v[i].p1].z == 0) | ((float)p[v[i].p2].z == 0) | ((float)p[v[i].p3].z == 0))
      {
         glColor3f( 0/255., 0/255., 153/255.);
      }
      else if( ((float)p[v[i].p1].z == -100) | ((float)p[v[i].p2].z == -100) | ((float)p[v[i].p3].z == -100))
      {
         glColor3f( 0/255., 0/255., 102/255.);
      }
      else if( ((float)p[v[i].p1].z == -200) | ((float)p[v[i].p2].z == -200) | ((float)p[v[i].p3].z == -200))
      {
         glColor3f( 0/255., 0/255., 77/255.);
      }
      else if( ((float)p[v[i].p1].z == -300) | ((float)p[v[i].p2].z == -300) | ((float)p[v[i].p3].z == -300))
      {
         glColor3f( 0/255., 0/255., 26/255.);
      }

      else if( ((float)p[v[i].p1].z == -400) | ((float)p[v[i].p2].z == -400) | ((float)p[v[i].p3].z == -400))
      {
         glColor3f( 0., 0., 0.);
      }

        //glColor3f((float) i/ntri, (float) i/ntri, (float) i/ntri);
        glVertex3f(((float)p[v[i].p1].x / max_dim_size) * 2 - image->w / max_dim_size, ((float)p[v[i].p1].y / max_dim_size) * 2 + (image->h / max_dim_size),
                   (float)p[v[i].p1].z / (float)max);
        glVertex3f(((float)p[v[i].p2].x / max_dim_size) * 2 - image->w / max_dim_size, ((float)p[v[i].p2].y / max_dim_size) * 2 + (image->h / max_dim_size),
                   (float)p[v[i].p2].z / (float)max);
        glVertex3f(((float)p[v[i].p3].x / max_dim_size) * 2 - image->w / max_dim_size, ((float)p[v[i].p3].y / max_dim_size) * 2 + (image->h / max_dim_size),
                   (float)p[v[i].p3].z / (float)max);
    }

    glEnd();
}
/*
void Draw_Triangles(int **bp, SDL_Surface *image)
{
    int numTriangleVertices;
    Attributes att = attr(image, bp);
    int max = biggest_height(image, bp);
    float max_dim_size = biggest_dim_size(image);
    
    for (i=0;i<ntri;i++) {
      printf("f3 %g %g %g %g %g %g %g %g %g 1 1 1\n",
         p[v[i].p1].x,p[v[i].p1].y,p[v[i].p1].z,
         p[v[i].p2].x,p[v[i].p2].y,p[v[i].p2].z,
         p[v[i].p3].x,p[v[i].p3].y,p[v[i].p3].z);
   }
    glBegin(GL_TRIANGLES);
    glColor3f(0.0f, 1.0f, 0.5f);
    for (int i = 0; i < ntri;i++)
    {
        glVertex3f(((float)p[v[i].p1].x / max_dim_size) * 2 - image->w / max_dim_size, ((float)p[v[i].p1].y / max_dim_size) * 2 + (image->h / max_dim_size),
                   (float)p[v[i].p1].z / (float)max);
                   glVertex3f(((float)p[v[i].p2].x / max_dim_size) * 2 - image->w / max_dim_size, ((float)p[v[i].p2].y / max_dim_size) * 2 + (image->h / max_dim_size),
                   (float)p[v[i].p2].z / (float)max);
                   glVertex3f(((float)p[v[i].p3].x / max_dim_size) * 2 - image->w / max_dim_size, ((float)p[v[i].p3].y / max_dim_size) * 2 + (image->h / max_dim_size),
                   (float)p[v[i].p3].z / (float)max);
    }

    glEnd();
}
*/

/* Free the memory allocated to bp after accessing it's data */
void free_bm(int **bp, SDL_Surface *image)
{
    for (int i = 0; i < image->w; i++)
    {

        free(bp[i]);
    }
    free(bp);
}
