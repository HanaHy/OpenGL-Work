//
//  main.cpp
//  HW2
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

void setPixel(float x, float y) {
  glVertex2f(x, y);
}

void lineBres (float x0, float y0, float xEnd, float yEnd)
{
  int dx = fabs (xEnd - x0),  dy = fabs(yEnd - y0);
  int p = 2 * dy - dx;
  int twoDy = 2 * dy,  twoDyMinusDx = 2 * (dy - dx);
  int x, y;
  
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
  setPixel (x, y);
  
  while (x < xEnd) {
    x++;
    if (p < 0)
      p += twoDy;
    else {
      y++;
      p += twoDyMinusDx;
    }
    setPixel (x, y);
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
  
  void setVertex(float a, float b) {
    x = a;
    y = b;
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
  //The middle vertex is always vertices[1]
  //int minX, maxX, minY, maxY;
  Triangle(float x0, float y0, float x1, float y1, float x2, float y2)
  {
    /* SET VERTICES IN ORDER OF Y INDEX - AT MOST 3 COMPARISONS */
   if( y0 <= y1)
   {
     if(y0 <= y2)
     {
       vertices[0].setVertex(x0, y0);
       if(y1 <= y2)
       {
         vertices[1].setVertex(x1, y1);
         vertices[2].setVertex(x2, y2);
       } // 0, 1, 2
       else
       {
         vertices[1].setVertex(x2, y2);
         vertices[2].setVertex(x1, y1);
       } // 0, 2, 1
     }
     else
     {
       vertices[0].setVertex(x2, y2);
       vertices[1].setVertex(x0, y0);
       vertices[2].setVertex(x1, y1);
     } //2, 0, 1
   }
   else {
     if(y2 <= y1)
     {
       vertices[0].setVertex(x2, y2);
       vertices[1].setVertex(x1, y1);
       vertices[2].setVertex(x0, y0);
     } // 2, 1, 0
     else
     {
       vertices[0].setVertex(x1, y1);
       if(y2 <= y0)
       {
         vertices[1].setVertex(x2, y2);
         vertices[2].setVertex(x0, y0);
       } //1, 2, 0
       else
       {
         vertices[1].setVertex(x0, y0);
         vertices[2].setVertex(x2, y2);
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
    
    
    
  }
  void drawTriangle()
  {
    glBegin(GL_POINTS);
    /*glVertex2f(vertices[0].x, vertices[0].y);
    glVertex2f(vertices[1].x, vertices[1].y);
    
    glVertex2f(vertices[1].x, vertices[1].y);
    glVertex2f(vertices[2].x, vertices[2].y);
    
    glVertex2f(vertices[0].x, vertices[0].y);
    glVertex2f(vertices[2].x, vertices[2].y);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glColor3f(0.0, 0.4, 0.2);*/
    
    int diff = vertices[1].y - vertices[0].y;
    for(int i = 1; i <= diff; i++)
    {
   // glVertex2f(vertices[0].x + i*edges[1].slope, vertices[0].y + i);
    //  glVertex2f(vertices[0].x + i*edges[0].slope, vertices[0].y + i);
      lineBres(vertices[0].x + i*edges[1].slope, vertices[0].y+i, vertices[0].x + i*edges[0].slope, vertices[0].y + i);
    }
    
    diff = vertices[2].y - vertices[1].y;
    for(int i = 1; i <= diff; i++)
    {
      //glVertex2f(vertices[2].x - i*edges[1].slope, vertices[2].y - i);
      //glVertex2f(vertices[2].x - i*edges[2].slope, vertices[2].y - i);
      lineBres(vertices[2].x - i*edges[1].slope, vertices[2].y - i, vertices[2].x - i*edges[2].slope, vertices[2].y - i);
    }
    
    glEnd();
    
    
    glFlush();

  }
  
};

void init (void)
{
  glClearColor(48.0/255.0, 71.0/255.0, 94.0/255.0, 1.0); // set background to black
  
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0.0, 400.0, 0.0, 400.0);
}

void makeTriangle (void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(0.0, 164.0/255.0, 297.0/255.0);
  Triangle *triangle1 = new Triangle(250, 350, 200, 300, 300, 300);
  
  triangle1 -> drawTriangle();
  
  //glColor3f(0.0, 164.0/255.0, 297.0/255.0);
  Triangle *triangle4 = new Triangle(150, 350, 100, 300, 200, 300);
  
  triangle4 -> drawTriangle();
  
  glColor3f(0.0, 198.0/255.0, 235.0/255.0);
  Triangle *triangle5 = new Triangle(150, 350, 200, 300, 250, 350);
  
  triangle5 -> drawTriangle();
  
  Triangle *triangle11 = new Triangle(175, 250, 200, 300, 225, 250);
  
  triangle11 -> drawTriangle();
  
  glColor3f(0.0, 172.0/255.0, 216.0/255.0);
  Triangle *triangle6 = new Triangle(150, 350, 150, 300, 200, 300);
  
  triangle6 -> drawTriangle();
  
  Triangle *triangle7 = new Triangle(250, 350, 250, 300, 200, 300);
  
  triangle7 -> drawTriangle();
  
  glColor3f(0.0, 153.0/255.0, 189.0/255.0);
  Triangle *triangle8 = new Triangle(200, 350, 175, 325, 225, 325);
  
  triangle8 -> drawTriangle();
  glColor3f(0.0, 164.0/255.0, 197.0/255.0);
  Triangle *triangle9 = new Triangle(200, 350, 250, 350, 225, 325);
  
  triangle9 -> drawTriangle();
  
  Triangle *triangle10 = new Triangle(150, 350, 200, 350, 175, 325);
  
  triangle10 -> drawTriangle();
  
  glColor3f(0.0, 159.0/255.0, 197.0/255.0);
  Triangle *triangle12 = new Triangle(200, 300, 175, 250, 150, 300);
  
  triangle12 -> drawTriangle();
  
  Triangle *triangle13 = new Triangle(250, 300, 225, 250, 200, 300);
  
  triangle13 -> drawTriangle();
  
  Triangle *triangle14 = new Triangle(175, 250, 225, 250, 200, 200);
  
  triangle14 -> drawTriangle();
  
  glColor3f(0.0, 174.0/255.0, 214.0/255.0);
  Triangle *triangle15 = new Triangle(100, 300, 200, 200, 150, 300);
  
  triangle15 -> drawTriangle();
  
  glColor3f(0.0, 126.0/255.0, 157.0/255.0);
  Triangle *triangle16 = new Triangle(250, 300, 200, 200, 300, 300);
  
  triangle16 -> drawTriangle();
  
  //glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(0.0, 186.0/255.0, 156.0/255.0);
  
  Triangle *triangle3 = new Triangle(200, 20, 50, 120, 350, 120);
  
  triangle3 -> drawTriangle();
  
  glColor3f(0.0, 106.0/255.0, 102.0/255.0);
  
  Triangle *triangle2 = new Triangle(200, 200, 50, 100, 350, 100);
  
  triangle2 -> drawTriangle();
  
  /*glBegin(GL_LINES);
  glVertex2f(triangle1->vertices[0].x, triangle1->vertices[0].y);
  glVertex2f(triangle1->vertices[1].x, triangle1->vertices[1].y);
  
  glVertex2f(triangle1->vertices[1].x, triangle1->vertices[1].y);
  glVertex2f(triangle1->vertices[2].x, triangle1->vertices[2].y);
  
  glVertex2f(triangle1->vertices[0].x, triangle1->vertices[0].y);
  glVertex2f(triangle1->vertices[2].x, triangle1->vertices[2].y);
  
  glEnd();
  
  
  glFlush();*/

}


int main (int argc, char** argv)
{
  glutInit(&argc, argv);
  
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  
  glutInitWindowPosition(50, 100);
  
  glutInitWindowSize(400, 400);
  
  glutCreateWindow("ASSIGNMENT 2");
  
  init ();
  
 glutDisplayFunc(makeTriangle);
  
  glutMainLoop();
  
  return 0;
}
