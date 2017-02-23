//
//  main.cpp
//  HW1
//
//  Created by Hana Hyder on 1/10/16.
//  Copyright © 2016 UCD. All rights reserved.
//

#include <iostream>

/* Remove for submission */

#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include <math.h>

#define PI 3.14159265

//#include <stdlib.h>

/* #include <GL/glut.h> // unmark for submission */


void init (void)
{
  glClearColor(0, 0, 0, 0.0); // set background to black
  
  glMatrixMode(GL_PROJECTION);
  gluOrtho2D(0.0, 400.0, 0.0, 400.0);
}

void setPixel(int x, int y) {
  glVertex2i(x, y);
}


void lineBres (int x0, int y0, int xEnd, int yEnd)
{
  int dx = fabs (xEnd - x0),  dy = fabs(yEnd - y0);
  int p = 2 * dy - dx;
  int twoDy = 2 * dy,  twoDyMinusDx = 2 * (dy - dx);
  int x, y;
  
  /* Determine which endpoint to use as start position.  */
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
}

void lineBresDown (int x0, int y0, int xEnd, int yEnd)
{
  int dx = fabs (xEnd - x0),  dy = fabs(yEnd - y0);
  int p = 2 * dy - dx;
  int twoDy = 2 * dy,  twoDyMinusDx = 2 * (dy - dx);
  int x, y;
  
  /* Determine which endpoint to use as start position.  */
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
      y--;
      p += twoDyMinusDx;
    }
    setPixel (x, y);
  }
}

void steepLineBres (int x0, int y0, int xEnd, int yEnd)
{
  int dx = fabs (xEnd - x0),  dy = fabs(yEnd - y0);
  int p = 2 * dx - dy;
  int twoDx = 2 * dx,  twoDxMinusDy = 2 * (dx - dy);
  int x, y;
  
  /* Determine which endpoint to use as start position.  */
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
  
  while (y < yEnd) {
    y++;
    if (p < 0)
      p += twoDx;
    else {
      x++;
      p += twoDxMinusDy;
    }
    setPixel (x, y);
  }
}

void steepLineBresDown (int x0, int y0, int xEnd, int yEnd)
{
  int dx = fabs (xEnd - x0),  dy = fabs(yEnd - y0);
  int p = 2 * dx - dy;
  int twoDx = 2 * dx,  twoDxMinusDy = 2 * (dx - dy);
  int x, y;
  
  /* Determine which endpoint to use as start position.  */
  if (y0 < yEnd) {
    x = xEnd;
    y = yEnd;
    xEnd = x0;
  }
  else {
    x = x0;
    y = y0;
  }
  
  setPixel(x, y);
  
  while (y > yEnd) {
    y--;
    if (p < 0)
      p += twoDx;
    else {
      x++;
      p += twoDxMinusDy;
    }
    setPixel (x, y);
  }
}



void lineSegment (void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(0.0, 0.4, 0.2);
  glBegin(GL_POINTS);

  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(0.1, 0.66, 0.72);
  
  lineBres(0, 200, 400, 200); // "x-axis"
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(0.14, 0.46, 0.8);
  
  steepLineBres(200, 0, 200, 400); // "y-axis"
  

  
  /* 15 deg */
  
  lineBres(7, 148, 393, 252);
  lineBresDown(7, 252, 393, 148);
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(0.51, 0.21, 0.66);
  
  
  /* 30 deg */
  
  lineBres(27, 100, 373, 300);
  lineBresDown(27, 300, 373, 100);
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(0.80, 0.11, 0.68);
  
  
  /* 45 deg */
  
  lineBres(59, 59, 341, 341);
  lineBresDown(59, 341, 341, 59);
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(0.14, 0.46, 0.8);
  
  /* 60 deg */
  steepLineBres(100, 27, 300, 373);
  steepLineBresDown(100, 373, 300, 27);
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(0.51, 0.21, 0.66);
  
  /* 75 deg */
  steepLineBres(148, 7, 252, 393);
  steepLineBresDown(148, 393, 252, 7);
  
  glClear(GL_COLOR_BUFFER_BIT);
  
  glColor3f(1.0, 1.0, 1.0);
  
  lineBresDown(178, 280, 253, 265);
  steepLineBresDown(178, 280, 235, 192);
  lineBres(235, 192, 297, 193);
  steepLineBresDown(253, 265, 297, 193);
  
  steepLineBres(200,200, 253, 265);
  
  steepLineBres(134, 205, 178, 280);
  steepLineBres(180, 130, 235, 192);
  
  steepLineBresDown(134, 205, 180, 130);
  
  lineBresDown(134, 205, 200, 200);
  
  steepLineBresDown(200, 200, 256, 135);
  steepLineBres(256, 135, 297, 193);
  
  lineBres(179, 130, 255, 135);
  
  
  glEnd();
  
  glFlush();
}



int main (int argc, char** argv)
{
  glutInit(&argc, argv);
  
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  
  glutInitWindowPosition(50, 100);
  
  glutInitWindowSize(400, 400);
  
  glutCreateWindow("EXAMPLE PROGRAM");
  
  init ();
  
  glutDisplayFunc(lineSegment);
  glutMainLoop();
  
  return 0;
}

/*float *PixelBuffer;
void display();
void lineSegment();
int main(int argc, char *argv[])
{
  //allocate new pixel buffer, need initialization!!
  PixelBuffer = new float[200 * 200 * 3];
  
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE);
  
  //set window size to 200*200
  glutInitWindowSize(200, 200);
  //set window position
  glutInitWindowPosition(100, 100);
  
  //create and set main window title
  int MainWindow = glutCreateWindow("Hello Graphics!!");
  glClearColor(0, 0, 0, 0); //clears the buffer of OpenGL
  //sets display function
  //glutDisplayFunc(display);
  glutDisplayFunc(lineSegment);
  
  glutMainLoop();//main display loop, will display until terminate
  return 0;
}

//main display loop, this function will be called again and again by OpenGL
void display()
{
  //Misc.
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  
  //draws pixel on screen, width and height must match pixel buffer dimension
  glDrawPixels(200, 200, GL_RGB, GL_FLOAT, PixelBuffer);
  
  //lineSegment();
  
  // glColor3f(0.0, 0.4, 0.2); //RGBy
  
  //window refresh
  glFlush();
}

void lineSegment()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(0.2, 0.6, 0.4);
  glBegin(GL_LINES);
  glVertex2i(10, 50);
  glVertex2i(40, 10);
  glEnd();
  glFlush();
}*/