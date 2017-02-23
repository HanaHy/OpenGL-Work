//
//  main.cpp
//  HW3
//
//  Created by Hana Hyder on 1/21/16.
//  Copyright © 2016 UCD. All rights reserved.
//


#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <math.h>

/* #include <GL/glut.h> */

#include <iostream>

float zBuffer[200][200];


void setPixel(float x, float y) {
  glVertex2f(x, y);
}

void lineBres (float x0, float y0, float xEnd, float yEnd, float normal[4])
{
  int dx = fabs (xEnd - x0),  dy = fabs(yEnd - y0);
  int p = 2 * dy - dx;
  int twoDy = 2 * dy,  twoDyMinusDx = 2 * (dy - dx);
  int x, y;
  float temp = -1*(normal[3] + normal[0]*x0 + normal[1]*(y0))/normal[2];
  
  /// Determine which endpoint to use as start position.
  if (x0 > xEnd) {
    x = xEnd;
    y = yEnd;
    xEnd = x0;
  }
  else {
    x = x0;
    y = y0;
  }
  if(zBuffer[x][y] == temp)
  {
    setPixel (x, y);
  }
  
  while (x < xEnd) {
    x++;
    if (p < 0)
      p += twoDy;
    else {
      y++;
      p += twoDyMinusDx;
    }
    temp = -1*(normal[3] + normal[0]*x + normal[1]*(y))/normal[2];
    if(zBuffer[x][y] == temp) {
    setPixel (x, y);
    }
  }
  /*float x = x0;
   int y = y0;
   
   while(x < xEnd)
   {
   setPixel(x, y);
   // setPixel(x+0.5, y+0.5);
   x++;
   }*/
}



class Vertex
{
  
public:
  float x;
  float y;
  float z;
  
  void setVertex(float a, float b, float c) {
    x = a;
    y = b;
    z = c;
  }
  
};

class Edge
{
public:
  float slope;
  bool isActive;
  
  void setSlope(float s) {
    slope = s;
  }
  void setActiveState(bool f)
  {
    isActive = f;
  }
  
}; //Dy is 1.

class Triangle
{
public:
  Vertex vertices[3]; // Vertex[0], Vertex[1], Vertex[2]
  Edge edges[3];
  float normal[4];
  Triangle(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2)
  {
    /* SET VERTICES IN ORDER OF Y INDEX - AT MOST 3 COMPARISONS */
    if( y0 <= y1)
    {
      if(y0 <= y2)
      {
        vertices[0].setVertex(x0, y0, z0);
        if(y1 <= y2)
        {
          vertices[1].setVertex(x1, y1, z1);
          vertices[2].setVertex(x2, y2, z2);
        } // 0, 1, 2
        else
        {
          vertices[1].setVertex(x2, y2, z2);
          vertices[2].setVertex(x1, y1, z1);
        } // 0, 2, 1
      }
      else
      {
        vertices[0].setVertex(x2, y2, z2);
        vertices[1].setVertex(x0, y0, z0);
        vertices[2].setVertex(x1, y1, z1);
      } //2, 0, 1
    }
    else {
      if(y2 <= y1)
      {
        vertices[0].setVertex(x2, y2, z2);
        vertices[1].setVertex(x1, y1, z1);
        vertices[2].setVertex(x0, y0, z0);
      } // 2, 1, 0
      else
      {
        vertices[0].setVertex(x1, y1, z1);
        if(y2 <= y0)
        {
          vertices[1].setVertex(x2, y2, z2);
          vertices[2].setVertex(x0, y0, z0);
        } //1, 2, 0
        else
        {
          vertices[1].setVertex(x0, y0, z0);
          vertices[2].setVertex(x2, y2, z2);
        } // 1, 0, 2
        
      } // 1 , ?, ?
    } // 1 < 0
    
    /* SET THE EDGES */
    
    /*
     
     edges[0].slope = slope from 1 to 0
     edges[1].slope = slope from 2 to 0
     edges[2].slope = slope from 1 to 2
     
     */
    
    edges[0].setSlope((vertices[1].x - vertices[0].x)/(vertices[1].y - vertices[0].y));
    edges[1].setSlope((vertices[2].x - vertices[0].x)/(vertices[2].y - vertices[0].y));
    edges[2].setSlope((vertices[2].x - vertices[1].x)/(vertices[2].y - vertices[1].y));
    
    /*std::cout << "The vertices are as follows: " << std::endl;
     std::cout << vertices[0].x << ", "<< vertices[0].y << std::endl;
     std::cout << vertices[1].x << ", "<< vertices[1].y << std::endl;
     std::cout << vertices[2].x << ", "<< vertices[2].y << std::endl << std::endl;
     
     std::cout << "The slopes are as follows: " << std::endl;
     std::cout << edges[0].slope << std::endl;
     std::cout << edges[1].slope << std::endl;
     std::cout << edges[2].slope << std::endl;*/
    
    /* normal[0] = A -> ByCz - BzCy
      normal[1] = B -> BzCx - BxCz
     normal[2] = C -> BxCy - ByCx
     
     By = vertices[2].y - vertices[0].y
     Bx = vertices[2].x - vertices[0].x
     Bz = vertices[2].z - vertices[0].z
     
     Cy = vertices[1].y - vertices[0].y
     Cx = vertices[1].x - vertices[0].x
     Cz = vertices[1].z - vertices[0].z
     
     normal[3] = -(A*x0 + B*y0 + C*z0) -> -1*(normal[0]*x0 + normal[1]*y0 + normal[2]*z0);
     
     */
    int Bx, By, Bz, Cx, Cy, Cz;
    
    By = vertices[2].y - vertices[0].y;
    Bx = vertices[2].x - vertices[0].x;
    Bz = vertices[2].z - vertices[0].z;
    
    Cy = vertices[1].y - vertices[0].y;
    Cx = vertices[1].x - vertices[0].x;
    Cz = vertices[1].z - vertices[0].z;
    
    normal[0] = By*Cz - Bz*Cy;
    normal[1] = Bz*Cx - Bx*Cz;
    normal[2] = Bx*Cy - By*Cx;
    
    normal[3] = -1*(normal[0]*x0 + normal[1]*y0 + normal[2]*z0);
    
    //std::cout << "The normal is as follows: " << std::endl;
    //std::cout << normal[0] << "x + " << normal[1] << "y + " << normal[2] << "z + " << normal[3] << " = 0 " << std::endl;
    
    /* POPULATE z-buffer */
    
    int diff = vertices[1].y - vertices[0].y;
    for(int i = 1; i <= diff; i++)
    {
      //lineBres(vertices[0].x + i*edges[1].slope, vertices[0].y+i, vertices[0].x + i*edges[0].slope, vertices[0].y + i, normal);
      int x = vertices[0].x + i*edges[1].slope;
      int y = vertices[0].y + i;
      int xEnd = vertices[0].x + i*edges[0].slope;
      float temp = -1*(normal[3] + normal[0]*(xEnd) + normal[1]*(y))/normal[2];
      if(xEnd > x)
      {
        int tmp = x;
        x = xEnd;
        xEnd = tmp;
      }
      while(xEnd <= x) {
        //std::cout << "Entered this loop" << std::endl;
      if(zBuffer[xEnd][y] < temp)
      {
        zBuffer[xEnd][y] = temp;
      }
        xEnd++;
        temp = -1*(normal[3] + normal[0]*(xEnd) + normal[1]*(y))/normal[2];
      }
      
    }
    
    diff = vertices[2].y - vertices[1].y;
    for(int i = 1; i <= diff; i++)
    {
     // lineBres(vertices[2].x - i*edges[1].slope, vertices[2].y - i, vertices[2].x - i*edges[2].slope, vertices[2].y - i, normal);
      
      int x = vertices[2].x - i*edges[1].slope;
      int y = vertices[2].y - i;
      int xEnd = vertices[2].x - i*edges[2].slope;
      float temp = -1*(normal[3] + normal[0]*(xEnd) + normal[1]*(y))/normal[2];
      if(xEnd > x)
      {
        int tmp = x;
        x = xEnd;
        xEnd = tmp;
      }
      while(xEnd <= x) {
        //std::cout << "Entered this loop" << std::endl;
        if(zBuffer[xEnd][y] < temp)
        {
          zBuffer[xEnd][y] = temp;
        }
        xEnd++;
        temp = -1*(normal[3] + normal[0]*(xEnd) + normal[1]*(y))/normal[2];
      }
      
      
    }
    
    
  }
  void drawTriangle()
  {
    glBegin(GL_POINTS);
 
    
    int diff = vertices[1].y - vertices[0].y;
    for(int i = 1; i <= diff; i++)
    {
      lineBres(vertices[0].x + i*edges[1].slope, vertices[0].y+i, vertices[0].x + i*edges[0].slope, vertices[0].y + i, normal);

    }
    
    diff = vertices[2].y - vertices[1].y;
    for(int i = 1; i <= diff; i++)
    {
      lineBres(vertices[2].x - i*edges[1].slope, vertices[2].y - i, vertices[2].x - i*edges[2].slope, vertices[2].y - i, normal);
    }
    
    glEnd();
    
    
    glFlush();
    
  }
  
};

void init (void)
{
  glClearColor(255.0/255.0, 133.0/255.0, 56.0/255.0, 1.0); // set background to black
  
  //glClearColor(0.0, 0.0, 0.0, 1.0);
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0.0, 200.0, 0.0, 200.0);
}

void makeTriangle (void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(117.0/255.0, 123.0/255.0, 135.0/255.0);
  Triangle *triangle1 = new Triangle(116, 95, 0, 135, 95, 0, 120, 79, 0 );
  
  triangle1 -> drawTriangle();
  
  glColor3f(89.0/255.0, 94.0/255.0, 116.0/255.0);
  Triangle *triangle2 = new Triangle(156, 85, 0, 135, 95, 0, 120, 79, 0 );
  
  triangle2 -> drawTriangle();
  
  glColor3f(102.0/255.0, 106.0/255.0, 116.0/255.0);
  Triangle *triangle3 = new Triangle(156, 85, 0, 166, 69, 0, 120, 79, 0 );
  
  triangle3 -> drawTriangle();
  
  glColor3f(50.0/255.0, 59.0/255.0, 60.0/255.0);
  Triangle *triangle4 = new Triangle(140, 60, 0, 166, 69, 0, 120, 79, 0 );
  
  triangle4 -> drawTriangle();

  glColor3f(83.0/255.0, 98.0/255.0, 110.0/255.0);
  Triangle *triangle5 = new Triangle(140, 60, 0, 100, 59, 10, 120, 79, 0 );
  
  triangle5 -> drawTriangle();
  
  glColor3f(67.0/255.0, 72.0/255.0, 94.0/255.0);
  Triangle *triangle6 = new Triangle(95, 80, 0, 100, 59, 10, 120, 79, 0 );
  
  triangle6 -> drawTriangle();

  glColor3f(87.0/255.0, 95.0/255.0, 111.0/255.0);
  Triangle *triangle7 = new Triangle(95, 80, 0, 116, 95, 0, 120, 79, 0 );
  
  triangle7 -> drawTriangle();

  glColor3f(59.0/255.0, 59.0/255.0, 59.0/255.0);
  Triangle *beak = new Triangle(95, 80, 10, 100, 59, 10, 80, 70, 10 );
  
  beak -> drawTriangle();
  
  glColor3f(0.0, 0.0/255.0, 0.0/255.0);
  Triangle *eye1 = new Triangle(120, 82, 50, 115, 76, 60, 125, 76, 60 );
  
  eye1 -> drawTriangle();
  Triangle *eye2 = new Triangle(120, 70, 50, 115, 76, 60, 125, 76, 60 );
  
  eye2 -> drawTriangle();

  
  glColor3f(205.0/255.0, 62.0/255.0, 34.0/255.0);
  Triangle *body1 = new Triangle(140, 60, 0, 166, 69, 0, 150, 45, 0 );
  
  body1 -> drawTriangle();
  glColor3f(221.0/255.0, 87.0/255.0, 41.0/255.0);
  Triangle *body2 = new Triangle(170, 50, 0, 166, 69, 0, 150, 45, 0 );
  
  body2 -> drawTriangle();
  
  glColor3f(174.0/255.0, 41.0/255.0, 18.0/255.0);
  Triangle *body3 = new Triangle(170, 50, 10, 166, 69, 0, 182, 53, 0 );
  
  body3 -> drawTriangle();
  
  glColor3f(244.0/255.0, 101.0/255.0, 64.0/255.0);
  Triangle *body4 = new Triangle(140, 60, 0, 102, 60, 10, 150, 45, 0 );
  
  body4 -> drawTriangle();
  
  glColor3f(227.0/255.0, 81.0/255.0, 51.0/255.0);
  Triangle *body5 = new Triangle(95, 40, 0, 102, 60, 0, 150, 45, 10 );
  
  body5 -> drawTriangle();
  
  glColor3f(206.0/255.0, 62.0/255.0, 33.0/255.0);
  Triangle *body6 = new Triangle(95, 40, 120, 115, 20, 60, 150, 45, 120 );
  
  body6 -> drawTriangle();
  
  glColor3f(173.0/255.0, 40.0/255.0, 30.0/255.0);
  Triangle *body7 = new Triangle(125, 10, 0, 115, 15, 0, 150, 35, 0 );
  
  body7 -> drawTriangle();
  
  glColor3f(246.0/255.0, 100.0/255.0, 64.0/255.0);
  Triangle *body8 = new Triangle(165, 20, 0, 175, 15, 0, 170, 35, 0 );
  
  body8 -> drawTriangle();

  
  //glColor3f(0.0, 198.0/255.0, 235.0/255.0);
  glColor3f(1.0, 1.0, 1.0);
  
  //Triangle *triangle7 = new Triangle(10, 50, 0, 70, 20, 3, 60, 80, 2 );
   Triangle *triangle8 = new Triangle(40, 40, 10, 20, 30, 0, 160, 30, 120 );
  
  triangle8 -> drawTriangle();
  
  Triangle *triangle9 = new Triangle(170, 160, 1, 150, 30, 0, 170, 30, 1 );
  
  triangle9 -> drawTriangle();
  
  Triangle *triangle10 = new Triangle(20, 180, 1, 60, 60, 1, 35, 175, 1);
  
  triangle10 -> drawTriangle();
  
  Triangle *triangle11 = new Triangle(70, 180, -2, 110, 60, -2, 85, 175, -2);
  
  triangle11 -> drawTriangle();
  
  Triangle *triangle12 = new Triangle(120, 180, -2, 160, 60, -2, 135, 175, -2);
  
  triangle12 -> drawTriangle();
  
}


int main (int argc, char** argv)
{
  glutInit(&argc, argv);
  
  for(int i = 0; i < 200; i++) {
    for(int j = 0; j < 200; j++) {
      zBuffer[i][j] = -3;
    }
  }
  
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  
  glutInitWindowPosition(50, 100);
  
  glutInitWindowSize(200, 200);
  
  glutCreateWindow("ASSIGNMENT 3");
  
  init ();
  
  glutDisplayFunc(makeTriangle);
  
  glutMainLoop();
  
  return 0;
}
