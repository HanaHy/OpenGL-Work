//
//  main.cpp
//  Project5
//
//  Created by Hana Hyder on 5/23/16.
//  Copyright © 2016 UCD. All rights reserved.
//

#define PI 3.14159265

#include <stdlib.h>
#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <iostream>
#include <math.h>
#include <time.h>

using namespace std; 

bool dotMode = false, holemode = false, gridMode = true; 
int once = 0, stptOnce = 0; 
int button, state, dragX, dragY, moveType = 0; 
double zoomX = 1.0; 
int width= 700, height= 500, isSpinning, tX = 0, tY = 0, tZ = 0, ttX = 0, ttY = 0, ttZ = 0, just_up = 0, 
tW = 0, ttW = 0; 
double winRadius= 660, transfac = .003, dX, dY, tt; 
int iterations = 0, delay = 4000; 
clock_t ticks1, ticks2, ticks3, ticks4; 


struct Point {
  float x; 
  float y; 
  float z; 
  
}; // Make struct instead for point manip

enum {
  SETPTS, PLOT
}viewpoint= SETPTS; 

Point **Points; 
float **weights; 
unsigned int dimX, dimY, netSize = 25; // default 25x25 points calculated

float *Uknots; 
float *Vknots; 

/* CALCULATIONS */
float coefficients(const float* Knots, float u, int i, int k); 
Point findVal(float u, float v); 
Point findValU(float t, int row); 
Point findValV(float t, Point* pnts); 

/* DISPLAY */

void plotControlPoints(); 
void setGround(); 
void showMenu(); 

void reshape(int width1, int height1)
{
  if (height1 == 0) {
    height1 = 1;
  }
  
  glViewport(0, 0, width1, height1);
  
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  
  gluPerspective(45, (float)width1/height1, 0.1, 100);
  
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 
}

void doTranslation(int tX, int tY, int tZ) {
  float m[16]; 
  glMatrixMode(GL_MODELVIEW); 
  glGetFloatv(GL_MODELVIEW_MATRIX, m); 
  glLoadIdentity(); 
  glTranslatef(transfac*tX, transfac*tY, transfac*tZ); 
  glMultMatrixf(m); 
}


void myRotated(double a, double X, double Y, double Z) {

  float m[16];
  glMatrixMode(GL_MODELVIEW); 
  glGetFloatv(GL_MODELVIEW_MATRIX, m); 
  glLoadIdentity(); 
  
  glTranslatef(transfac*ttX, transfac*ttY, transfac*ttZ); 
  glRotated(a, X, Y, Z); 
  glTranslatef(-transfac*ttX, -transfac*ttY, -transfac*ttZ); 
  glMultMatrixf(m); 
}


void doRotation(double dX, double dY) {
  double offX = 2.0*(double)dragX/(double)width-1.0; 
  double offY = 1.0-2.0*(double)dragY/(double)height; 
  double offZ = 0.25; 
  double X, Y, Z, a;
  X = -offZ*dY; 
  Y = offZ*dX; 
  Z = offX*dY - offY*dX; 
  a = 180.*1.5*sqrt(dX*dX + dY*dY)/(PI*winRadius); 
  myRotated(a, X, Y, Z); 
  return; 
}

Point findVal(float u, float v)
{
  Point arr[4];
  
  for(int i = 0; i < 4; i++)
  {
    // do U calc first then do V calc.
    arr[i] = findValU(u, i);
  }
  
  return findValV(v, arr);
}


void display (void)
{

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  
  if(viewpoint == SETPTS)
  {
    if(stptOnce == 0)
    {
      int tmp; 
      cout << "SETPTS MODE: ENTER VALUES" << endl; 
      cout << "Enter dimX: ";
      cin >> dimX;
      cout << "Enter dimY: ";
      cin >> dimY;
      
      
      Points = new Point*[dimX];
      
      for(int i = 0; i < dimX; i++)
      {
        Points[i] = new Point[dimY];
      }
      
      cout << "NOW ENTERING CONTROL POINTS-------" << endl; 
      
      for(int i = 0; i < dimX; i++)
      {
        for(int j = 0; j < dimY; j++)
        {
          cout << "Entering for Point[" << i << "][" << j << "]: " << endl; 
          //cout << "x value: ";
          cin >> Points[i][j].x >> Points[i][j].y >> Points[i][j].z;
          //cout << "y value: ";
         // cin >> Points[i][j].y;
          //cout << "z value: ";
          //cin >> Points[i][j].z;
        }
      }
      
      stptOnce++; 
      
      viewpoint= PLOT; 
    }
  }
  
  
  if(viewpoint == PLOT)
  {
  Point net[netSize][netSize]; 

    if(once == 0)
    {
      showMenu(); 
    }
  if(once != 2)
  {
    glScalef(0.3, 0.3, 0.3); 
    once++; 
  }
  
    glPushMatrix(); 
    glScalef(zoomX, zoomX, zoomX); 
  
  int count = 0; 
  int i, j; 
  
  glColor3f(1, 0, 1); 
  
  ticks4 = clock(); 
  tt = (double)(ticks4 - ticks3)/(double)CLOCKS_PER_SEC; 
  if(state == GLUT_DOWN && tt > .05) isSpinning = 0; 
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
  j = iterations - delay; 
  glColor4f(1., .2, 0.2, .7); 
  if(isSpinning) {
    glMatrixMode(GL_MODELVIEW); 
    doRotation(dX, dY); 
  }

  plotControlPoints(); 
  
  if(gridMode)
    setGround(); 
  
  
  glPointSize(2); 
  glShadeModel(GL_SMOOTH); 
  glBegin(GL_POINTS); 
  for(i = 0; i < netSize; ++i)
  {
    
    float u = ((float)i)/(netSize - 1);
    
    //glColor3f(1, 0, 1); 
    
    for(j = 0; j < netSize; ++j) {
      
      glColor3f((double)j/netSize, 0, 1); 
      
      float v = ((float)j)/(netSize - 1);
      
      Point p = findVal(u, v); 
      
      // draw point
      if(dotMode)
      {
        glVertex3f(p.x, p.y, p.z); 
      }
      
      net[i][j].x = p.x; 
      net[i][j].y = p.y; 
      net[i][j].z = p.z; 
      count++; 
    }
  }
  
  
  //cout << "THERE ARE " << count << " POINTS and netSize is " << netSize << endl; 
  
  glEnd(); 
  
  
  if(!dotMode)
  {
  glShadeModel(GL_SMOOTH); 
  glBegin(GL_QUADS); 
  glColor3f(1, 1, 1); 
  
  for(i = 0; i < netSize-1; i++)
  {
    for(j = 0; j < netSize -1; j++)
    {
      //double tmp = fmax((double)j/netSize, 0.2); 
      //glColor3f(tmp, tmp, tmp); 
      
      /* SHOWS HOLES */
      if(holemode)
      {
      glColor3f((double)j/netSize, 0, 1); 
      glVertex3f(net[i][j].x, net[i][j].y, net[i][j].z); //Top left
      glVertex3f(net[i+1][j].x, net[i+1][j].y, net[i+1][j].z); //Bottom left
      glColor3f((double)(j+1)/netSize, 0, 1); 
      glVertex3f(net[i][j+1].x, net[i][j+1].y, net[i][j+1].z); //Top right
      glVertex3f(net[i+1][j+1].x, net[i+1][j+1].y, net[i+1][j+1].z); //Bottom right
      }
      else
      {
       glColor3f((double)j/netSize, 0, 1); 
      glVertex3f(net[i][j].x, net[i][j].y, net[i][j].z); //Top left
      glColor3f((double)(j+1)/netSize, 0, 1); 
      glVertex3f(net[i][j+1].x, net[i][j+1].y, net[i][j+1].z); //Top right
      glVertex3f(net[i+1][j+1].x, net[i+1][j+1].y, net[i+1][j+1].z); //Bottom right
      glColor3f((double)j/netSize, 0, 1); 
      glVertex3f(net[i+1][j].x, net[i+1][j].y, net[i+1][j].z); //Bottom left
      }
      
    }
  }
  
  glEnd(); 
  }
  
  glPopMatrix(); 
  glFlush(); 
  ++iterations; 
  }
}

Point findValU(float u, int spot)
{
  float tmp1fl = (1 - u);
  
  float coeff1 = powf(u, 3.0);
  float coeff2 = 3*powf(u, 2.0)*tmp1fl;
  float coeff3 = 3*u*powf(tmp1fl, 2.0);
  float coeff4 = powf(tmp1fl, 3.0);
  
  Point temp;
  
  // 4 pts in bez part
  
  temp.x = coeff1*Points[spot][0].x + coeff2*Points[spot][1].x + coeff3*Points[spot][2].x + coeff4*Points[spot][3].x;
  temp.y = coeff1*Points[spot][0].y + coeff2*Points[spot][1].y + coeff3*Points[spot][2].y + coeff4*Points[spot][3].y;
  temp.z = coeff1*Points[spot][0].z + coeff2*Points[spot][1].z + coeff3*Points[spot][2].z + coeff4*Points[spot][3].z;
  
  return temp;
}

Point findValV(float u, Point * arrPt)
{
  float tmp1fl = 1 - u;
  
  float coeff1 = powf(u, 3.0);
  float coeff2 = 3*powf(u, 2.0)*tmp1fl;
  float coeff3 = 3*u*powf(tmp1fl, 2.0);
  float coeff4 = powf(tmp1fl, 3.0);
  
  Point temp;
  
  temp.x = coeff1*arrPt[0].x + coeff2*arrPt[1].x + coeff3*arrPt[2].x + coeff4*arrPt[3].x;
  temp.y = coeff1*arrPt[0].y + coeff2*arrPt[1].y + coeff3*arrPt[2].y + coeff4*arrPt[3].y;
  temp.z = coeff1*arrPt[0].z + coeff2*arrPt[1].z + coeff3*arrPt[2].z + coeff4*arrPt[3].z;
  
  
  return temp;
}

void plotControlPoints()
{
  glBegin(GL_LINES); 
  glColor3f(1, 0, 0); 
  for(int i = 0; i < dimX; i++)
  {
    for(int j = 0; j < dimY; j++)
    {
      if(j != dimY-1)
      {
        glVertex3f(Points[i][j].x, Points[i][j].y, Points[i][j].z); 
        glVertex3f(Points[i][j+1].x, Points[i][j+1].y, Points[i][j+1].z); 
      }
      if(i != dimX-1)
      {
        glVertex3f(Points[i][j].x, Points[i][j].y, Points[i][j].z); 
        glVertex3f(Points[i+1][j].x, Points[i+1][j].y, Points[i+1][j].z); 
      }
    }
  }
  glEnd(); 
}

void setGround()
{
  glBegin(GL_LINES); 
  glColor3f(1, 1, 1); 
  for(int i = -50; i < 50; i++)
  {
    glVertex3f(-20, -10, i); 
    glVertex3f(20, -10, i); 
    glVertex3f(i, -10, -20); 
    glVertex3f(i, -10, 20); 
  }
  
  
  glEnd(); 
  
  /*glBegin(GL_QUADS); 
  glColor3f(1, 1, 1); 
  
  --wrong glVertex3f(0, 0, 0); // X & Y
  glVertex3f(0, 50, 0); 
  glVertex3f(50, 50, 0); 
  glVertex3f(50, 0, 0); 
  
  glVertex3f(0, 0, 0); // X & Z
  glVertex3f(0, 0, 50); 
  glVertex3f(50, 0, 50); 
  glVertex3f(50, 0, 0); 
  
  
  glEnd(); */
  
}


void init() {
  // enable depth testing
  glEnable(GL_DEPTH_TEST); 
  /*GLfloat mat_diffuse[] = { 0.74, 0.36, 0.0, 1.0 }; 
  GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 }; 
  GLfloat mat_shininess[] = { 100.0 }; 
  GLfloat ambientLight[] = { 1.0, 1.0, 1.0 }; 
  
  glClearColor (0.0, 0.0, 0.0, 0.0); 
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse); 
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular); 
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess); 
  
  glEnable(GL_LIGHTING); 
  glEnable(GL_LIGHT0); 
  glEnable(GL_LIGHT1); 
  glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight); 
  //glEnable(GL_DEPTH_TEST); 
  glEnable(GL_AUTO_NORMAL); 
  glEnable(GL_NORMALIZE); 
  
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); */

}

float coefficients(const float* Knots, float u, int i, int k)
{
  
  if(k== 1)
  {
    if( u <= Knots[i+1] && Knots[i] <= u )
    {
      return 1.0f; 
    }
    else
      return 0.0f; 
  }
  
  float temp1 = -1.0*Knots[i] + Knots[i+k-1];
  float temp2 = -1.0*Knots[i+1] + Knots[i+k];
  
  float temp3 = 0;
  float temp4 = 0;
  
  if(temp2 > 0)
  {
    temp4 = (Knots[i+k]-u) / temp2 * coefficients(Knots, u, i+1, k-1); 
  }
  
  if(temp1 > 0)
  {
    temp3 = ((u-Knots[i]) / temp1) * coefficients(Knots, u, i, k-1);
  }
  return temp3 + temp4; 
}


void Exit() {
  delete [] Points; 
  delete [] weights; 
}

void keyPress(unsigned char key, int, int) {
  switch(key) {
      
      // increase the netSize
    case 'D':
    case 'd':
      dotMode = !dotMode; 
      cout << ">d:      DOT MODE: Status Changed" << endl; 
      break; 
    case 'G':
    case 'g':
      gridMode = !gridMode; 
      cout << ">g:      GRID MODE: Status Changed" << endl; 
      break; 
    case 'H':
    case 'h':
      holemode = !holemode; 
      cout << ">h:      HOLE MODE: Status Changed" << endl; 
      break; 
    case 'X':
    case 'x':
      cout << ">x:      DRAG STATE: Rotate Model in X" << endl; 
      moveType = 0; 
      glutPostRedisplay(); 
      break;
      
    case 'Y':
    case 'y':
      cout << ">y:      DRAG STATE: Rotate Model in Y" << endl; 
      moveType = 1; 
      glutPostRedisplay(); 
      break;
    case 'E':
      case 'e':
      viewpoint=SETPTS;
      break;
      
    case 'Z':
    case 'z':
      cout << ">z:      DRAG STATE: Rotate Model in Z" << endl; 
      moveType = 2; 
      glutPostRedisplay(); 
      break;
      
    case 'M':
    case 'm':
      cout << ">m:      MENU STATE: Display Menu" << endl; 
      showMenu(); 
      break;
      
    case 27:
    case 'Q':
    case 'q':
      exit(0);
      
    case 'j':
    case 'J':
      netSize++;
      break; 
      
      // decrease the netSize
    case 'f':
    case 'F':
      
      netSize--;
      
      if (netSize < 5)
        netSize = 5; 
      break;
      
    default:
      break; 
  }
  
  glutPostRedisplay(); 
}

void mouse(int but, int sta, int x, int y) {
  button = but; 
  state = sta; 
  if(state == GLUT_DOWN) {
    dragX = x; 
    dragY = y; 
    dX = 0; 
    dY = 0; 
  }
  if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) {
    /*
     dX = x - dragX; 
     dY = dragY - y; 
     if(dX!= 0||dY!= 0)
     isSpinning = 1; 
     else
     isSpinning = 0; 
     */
    ticks1 = clock(); 
    isSpinning = 0; 
    just_up = 1; 
  }
}
void motion(int x, int y) {
  //if (state == GLUT_DOWN && (button == GLUT_LEFT_BUTTON || moveType == 0)) {
  if (state == GLUT_DOWN && (moveType == 0)) {
    dX = x - dragX; 
    dY = dragY - y; 
    glMatrixMode(GL_MODELVIEW); 
    doRotation(dX, dY); 
    dragX = x; 
    dragY = y; 
  }
  //if (state == GLUT_DOWN && (button == GLUT_MIDDLE_BUTTON || moveType == 1)) {
  if (state == GLUT_DOWN && (moveType == 1)) {
    tX = x - dragX; 
    tY = dragY - y; 
    ttX += tX; 
    ttY += tY; 
    dragX = x; 
    dragY = y; 
    doTranslation(tX, tY, 0); 
  }
  //if (state == GLUT_DOWN && (button == GLUT_RIGHT_BUTTON || moveType == 2)) {
  if (state == GLUT_DOWN && (moveType == 2)) {
    tZ = x - dragX; 
    ttZ += tZ; 
    tW = y - dragY; 
    ttW += tW; 
    dragX = x; 
    dragY = y; 
    doTranslation(0, 0, tZ); 
  }
  ticks3 = clock(); 
  tt = (double)(ticks3 - ticks4)/(double)CLOCKS_PER_SEC; 
  display(); 
}
void passive_motion(int x, int y) {
  double t; 
  if(just_up) {
    ticks2 = clock(); 
    t = (double)(ticks2 - ticks1)/(double)CLOCKS_PER_SEC; 
    just_up = 0; 
    if (t < .01) {
      dX = .2*(x - dragX); 
      dY = .2*(dragY - y); 
      isSpinning = 1; 
    }
  }
}


void specialKeyInput(int key, int x, int y)
{
  if(key == GLUT_KEY_LEFT)
  {
    zoomX *= 0.8; 
    cout << ">^L:     ZOOM STATE: Zoom Out" << endl; 
  }
  if(key == GLUT_KEY_RIGHT)
  {
    zoomX *= 1.25; 
    cout << ">^R:     ZOOM STATE: Zoom In" << endl; 
  }
  
  glutPostRedisplay(); 
  
}

void showMenu()
{
  cout << "=================== INSTRUCTIONS =================== " << endl; 
  cout << "| Follow the instructions indicated on the screen. |" << endl; 
  cout << "| INPUTS:                                          |" << endl; 
  cout << "|      Left/Right Arrow: Zoom Out/Zoom In          |" << endl; 
  cout << "|      D or d          : Enable/Disable Dot Mode   |" << endl; 
  cout << "|      H or h          : Enable/Disable Hole Mode  |" << endl; 
  cout << "|      G or g          : Enable/Disable Grid Mode  |" << endl; 
  cout << "|      J or j          : Calculate More Points     |" << endl; 
  cout << "|      F or f          : Calculate Fewer Points    |" << endl; 
  cout << "|      Drag Mouse      : Rotate Model              |" << endl; 
  cout << "|      X or x          : Rotate Model in X         |" << endl; 
  cout << "|      Y or y          : Rotate Model in Y         |" << endl; 
  cout << "|      Z or z          : Rotate Model in Z         |" << endl;
  cout << "|                                                  |" << endl;
  cout << "|      E or e          : Enter New Values          |" << endl;
  cout << "|                                                  |" << endl; 
  cout << "|      M or m          : View Menu Options         |" << endl; 
  cout << "|      Q or q          : Quit                      |" << endl; 
  cout << "|                                                  |" << endl; 
  cout << "=================== INSTRUCTIONS =================== " << endl; 
  
}


int main(int argc, char** argv)
{
  
  glutInit(&argc, argv); 
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); 
  glutInitWindowSize(width, height); 
  glutCreateWindow("CUBIC BEZIER SURFACE");
  glutDisplayFunc(display); 
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyPress);
  glutSpecialFunc(specialKeyInput); 
  init();
  glutMouseFunc(mouse); 
  glutMotionFunc(motion); 
  glutPassiveMotionFunc(passive_motion); 
  atexit(Exit);
  glutMainLoop(); 
  
  return 0; 
}
