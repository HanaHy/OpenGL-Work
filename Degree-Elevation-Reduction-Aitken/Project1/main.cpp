//
//  main.cpp
//  Project2
//
//  Created by Hana Hyder on 4/23/16.
//  Copyright © 2016 UCD. All rights reserved.
//

#define PI 3.14159265

#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <list>
#include <ctime>
#include "/usr/local/include/Eigen/Dense"

using namespace std;
using namespace Eigen;

list< pair<int,int> > ctrlPoints;
list< pair<int,int> > ctrlPoints2; //treated as list of vectors
list< pair<int,int> >::iterator activePt;

float *ts;

//float **castelVals;
int univSize;
bool activate;
bool compute = false;
bool nextCurve = false;
int threshold = 90000;
static float u;
int xmin1, xmax1, ymin1, ymax1, xmin2, xmax2, ymin2, ymax2;


/*  Set initial size of the display window.  */
GLsizei winWidth = 600, winHeight = 600;

/*  Set size of world-coordinate clipping window.  */
GLfloat xwcMin = -300.0, xwcMax = 300.0;
GLfloat ywcMin = -300.0, ywcMax = 300.0;

/* Class Point */
class Point
{
public:
  float x;
  float y;
};

Point univPoint;
//Point *clist1;
//Point *clist2;

/*DISPLAYING*/
void display();
void menu();
void instructions(char *string);
void slider();
void atSlider();
void drawArrows();
void drawArrow(int x1, int y1, int destX, int destY);

/*PROCESSING*/
void processMouseClick(int button, int state, int x, int y);
void winReshapeFcn (GLint newWidth, GLint newHeight);
void keyInput(unsigned char key, int x, int y);
void specialKeyInput(int key, int x, int y);

/*CALCULATIONS*/
void linInterp();
void degUp();
void degDown();
void addToListCurve(Point points[], int size, float t, Point clist1[], Point clist2[]);
void pascalTriangle(GLint n, GLint *C);
void autoGenVec();

/* DRAWING */
void bezier (Point* ctrlPts, GLint nCtrlPts, GLint nBezCurvePts);
void drawCurveSeg(Point points[], int size, float t);
void drawCurve(Point points[], int size, float t);
void aitkenAlg(Point points[], int size, float t);
void aitkenAlgN(Point points[], int size, float t);
void aitkenAlgSeg(Point points[], int size, float t);

enum {
  SETPTS,ADJPTS,AITKEN,BERN,HERM,VECEN
}viewpoint=SETPTS;

int main(int argc, char *argv[])
{
  u = 0;
  activate = false;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition (50, 50);
  glutInitWindowSize (winWidth, winHeight);
  
  
 glutCreateWindow("Project 2 Curves");
  glutMouseFunc(processMouseClick);
  glClearColor(0.25, 0.75, 0.9, 1);
  glutReshapeFunc (winReshapeFcn);
  //sets display function
  glutDisplayFunc(display);
  glutKeyboardFunc(keyInput);
  glutSpecialFunc(specialKeyInput);
  
  glutMainLoop();//main display loop, will display until terminate
  return 0;
}

void plotPoint (int x, int y)
{
  glBegin (GL_POINTS);
  glVertex2f (x, y);
  glEnd ( );
}

void processMouseClick(int button, int state, int x, int y)
{
  x = -1*(300 - x);
  y = 300 - y;
  //cout << "x is: " << x << "and y is " << y << endl;
  
  switch(viewpoint)
  {
      /* USER PLOTS POINTS */
      case SETPTS:
      if(button == GLUT_LEFT_BUTTON)
      {
        
        if(state == GLUT_DOWN)
        {
          if((x <= 265 && x >= 220 && y <= -220 && y >= -250) || (x <= -220 && x>= -270 && y <= 270 && y >= 240))
          {
            viewpoint=ADJPTS;
            break;
          }
          if(y < 240) {
          ctrlPoints.emplace_back(make_pair(x, y));
          }
          else if(y >= 240)
          {
            if(x <= -135 && x >= -225)
            {
              viewpoint=BERN;
              break;
            }
            else if(x <= -50 && x >= -125)
            {
              viewpoint=AITKEN;
              break;
            }
            else if(x >= -40 && x <= 35)
            {
              viewpoint=HERM;
              break;
            }


          }
        }
      }
      break;
      /* USER MOVES POINTS */
      case ADJPTS:
      if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
          activate = false;
          
          if(y <= 290 && y >= 260)
          {//User hit something near the menu
            if(x <= -135 && x >= -225)
            {
              viewpoint=BERN;
              break;
            }
            else if(x <= -50 && x >= -125)
            {
              viewpoint=AITKEN;
              break;
            }
            else if(x >= -40 && x <= 35)
            {
              viewpoint=HERM;
              break;
            }
          }
          
          /* LOOP THROUGH LIST OF CTRLPOINTS */
          for(activePt = ctrlPoints.begin(); activePt != ctrlPoints.end(); ++activePt)
          {
            /* IF WITHIN 3x3 BOUNDS OF POINT */
            if(abs(activePt->first - x) <= 3 && abs(activePt->second - y) <= 3)
            {
              activate = true;
              break;
            } //Case within 3x3 bounds
          } // for loop
          //cout << activePt->first << endl;
        } //State GLUT_DOWN
        if(state == GLUT_UP)
        {
          
          if(y <= -220 && y >= -250)
          {
            if(x <= 265 && x >= 180)
            {
              if(activate)
              {
                ctrlPoints.erase(activePt);
                activate = false;
                ctrlPoints2.clear();
              }
            } //hit delete button
            if(x >= 135 && x<= 170)
            {
              viewpoint=SETPTS;
            } //user hit add button
          } //within button range

          if(activate)
          {
            activePt->first = x;
            activePt->second = y;
            activate = false;
          }
          activate = false;
        }
      }
      break;
      
    case AITKEN:
    case BERN:
      if(y <= 290 && y >= 260)
      {//User hit something near the menu
        if(x <= -235 && x>= -285)
        {
          viewpoint=ADJPTS;
          /*for (auto& x: ctrlPoints)
           cout << "(" << x.first << ", " << x.second << "), ";*/ //print statement
          break;
        } //User clicked "EDIT"
        else if(x <= -135 && x >= -225)
        {
          viewpoint=BERN;
          break;
        }
        else if(x <= -50 && x >= -125)
        {
          viewpoint=AITKEN;
          break;
        }
        else if(x >= -40 && x <= 35)
        {
          viewpoint=HERM;
        }

      }
      break;
      case HERM:
      if(y <= 290 && y >= 260)
      {//User hit something near the menu
        if(x <= -235 && x>= -285)
        {
          viewpoint=ADJPTS;
          /*for (auto& x: ctrlPoints)
           cout << "(" << x.first << ", " << x.second << "), ";*/ //print statement
          break;
        } //User clicked "EDIT"
        else if(x <= -135 && x >= -225)
        {
          viewpoint=BERN;
          break;
        }
        else if(x <= -50 && x >= -125)
        {
          viewpoint=AITKEN;
          break;
        }
        
                
      }
      else if(y <= -220 && y >= -250)
      {
        if(ctrlPoints2.size() < ctrlPoints.size())
        {
        if(x <= 180 && x >= 135)
        {
          activePt = ctrlPoints.begin();
          autoGenVec();
        } //user pressed 'auto' button
        else if(x <= 275 && x >= 190)
        {
          activePt = ctrlPoints.begin();
          ctrlPoints2.clear();
          viewpoint = VECEN;
          break;
        } //user pressed 'manual' button
        }
      }
      break;
    case VECEN:
      if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
      if(ctrlPoints2.size() < ctrlPoints.size())
      {
       // cout << "Click Pt  - x is: " << x << "and y is " << y << endl;
       // cout << "Active Pt - x is: " << activePt->first << "and y is " << activePt->second << endl;
       // cout << "Direc Vct - x is: " << x - activePt->first << "and y is " << y - activePt->second << endl;
        ctrlPoints2.emplace_back(make_pair(x - activePt->first, y - activePt->second));
        activePt++;
        if(ctrlPoints2.size() == ctrlPoints.size())
        {
          viewpoint = HERM;
          break;
        }
        
          }
          else
          {
            viewpoint = HERM;
          }
        }
      }
      
      break;
      
  }
  display();
}

void connectingLines()
{
  glBegin(GL_LINES);
  //list< pair<int,int> >::iterator tmpIt;
  int x1 = ctrlPoints.begin()->first;
  int y1 = ctrlPoints.begin()->second;
  int x2 = ctrlPoints.end()->first;
  int y2 = ctrlPoints.end()->second;
  
  for (auto& x: ctrlPoints)
  {
    if(!(x.first == x1 && x.second == y1) || (x.first == x2 && x.second == y2))
    {
      glVertex2i(x.first, x.second);
    }
    glVertex2i(x.first, x.second);
  }
  glEnd();
  
}

//main display loop, this function will be called again and again by OpenGL
void display()
{
  //Misc.
  glClear(GL_COLOR_BUFFER_BIT);
  menu(); //Creates menu.
  
  glColor3f(1,1,1);
  glPointSize(5);
  
  glBegin(GL_POINTS);
  bool tmp = true;
  /*for (auto& x: ctrlPoints)
    cout << "(" << x.first << ", " << x.second << "), ";
  
  cout << endl << endl;
  */
  
  /* DRAW POINTS OF CURVE 1*/
  for (auto& x: ctrlPoints)
  {
    if(activate) {
      if(x.first == activePt->first && x.second == activePt->second)
      {
        glColor3f(0,0,1); //different color for active point.
        glVertex2i(x.first, x.second);
        glColor3f(1, 1, 1);
        tmp = false;
      }
    }
    if(tmp)
      glVertex2i(x.first, x.second);
    tmp = true;
  }
  glColor3f(1, 1, 1);
  
  glEnd();
  univSize = (int)ctrlPoints.size();
//  if(viewpoint==INTER)
//    size2 = (int)ctrlPoints2.size();
  int size2 = univSize;
  Point points[size2]; // = new Point[univSize];
  Point miniPoints[4]; //Will store direction
  char tmp1[32];
  char tmp2[64] = "t = ";
  int i = 0;
  list< pair<int,int> >::iterator activePt2;
  ts = new float[size2]; //setting the size.
  float tmpVal = 1.0;
  if(size2 != 0)
    tmpVal = ((float)1)/((int)ctrlPoints.size() - 1);
  for(float incr = 0.0; incr <= 1.0; incr += tmpVal)
  {
    ts[i] = incr;
   // cout << incr << " ";
    i++;
  }
  
  /*if(viewpoint==HERM)
  {
    if(ctrlPoints.size() == ctrlPoints2.size())
    {
      tmpVal = ((float)1)/(4)
    }
  }*/
  
  //cout << endl << endl;
  
  i = 0;
  
  switch(viewpoint)
  {
    case SETPTS:
      glColor3f(0.25, 0.6, 1.0);
      glBegin(GL_QUADS);
      glVertex2i(220, -250);
      glVertex2i(220, -220);
      glVertex2i(265, -220);
      glVertex2i(265, -250);
      glEnd();
      glPushMatrix();
      glTranslatef(225, -240, 0);
      //glTranslatef(-250, 0);
      glScalef(0.1, 0.1, 0.1);
      glColor3f(1, 1, 1);
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'D');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'O');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'N');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'E');
      glPopMatrix();
      instructions("CLICK TO CREATE POINTS.");
      break;
      
    case ADJPTS:
      /* DRAW CONNECTING LINES */
      connectingLines();
      
      /* DRAW BUTTONS */
      glColor3f(0.25, 0.6, 1.0);
      glBegin(GL_QUADS);
      glVertex2i(135, -250);
      glVertex2i(135, -220);
      glVertex2i(170, -220);
      glVertex2i(170, -250);
      glEnd();
      glPushMatrix();
      glTranslatef(140, -240, 0);
      //glTranslatef(-250, 0);
      glScalef(0.1, 0.1, 0.1);
      glColor3f(1, 1, 1);
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'D');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'D');
      glPopMatrix();
      
      glColor3f(0.25, 0.6, 1.0);
      glBegin(GL_QUADS);
      glVertex2i(180, -250);
      glVertex2i(180, -220);
      glVertex2i(265, -220);
      glVertex2i(265, -250);
      glEnd();
      glPushMatrix();
      glTranslatef(200, -240, 0);
      //glTranslatef(-250, 0);
      glScalef(0.1, 0.1, 0.1);
      glColor3f(1, 1, 1);
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'T');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'R');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'S');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'H');
      //glutStrokeCharacter(GLUT_STROKE_ROMAN, 'E');
      glPopMatrix();
      /* DRAW INSTRUCTIONS */
      instructions("DRAG POINTS TO TRASH TO DELETE");
      break;
      
    case AITKEN:
      //connectingLines(); - don't need that this time.
      
      i = 0;
      for (auto& a: ctrlPoints)
      {
        points[i].x = a.first;
        points[i].y = a.second;
        i++;
      }
      atSlider();
      instructions("PRESS LEFT RIGHT KEYS TO CHANGE t.");
      sprintf(tmp1, "%0.3f", u);
      strcat(tmp2, tmp1);
      glPushMatrix();
      glTranslatef(0, 40, 0);
      instructions(tmp2);
      glPopMatrix();
      
      /* AITKEN ALGORITHM */
      glPushMatrix();
      glColor3f(0.5, 0.0, 1.0);
      
      glLineWidth(2);
      
      glBegin(GL_LINES);
      glEnable(GL_LINE_SMOOTH);
      
      for(float in = 0.0; in <= 1.0; in += 0.01)
      {
        if(in != 0.0 && in != 1.0)
        {
          aitkenAlg(points, univSize, in);
          i = 0;
          for (auto& a: ctrlPoints)
          {
            points[i].x = a.first;
            points[i].y = a.second;
            i++;
          }
        }
        aitkenAlg(points, univSize, in);
        i = 0;
        for (auto& a: ctrlPoints)
        {
          points[i].x = a.first;
          points[i].y = a.second;
          i++;
        }
      }
      //glLineWidth(1);
      glEnd();
      glLineWidth(1.f);
      glPopMatrix();
      
      
      glBegin(GL_POINTS);
      glColor3f(1, 1, 1);
      /* DRAW POINTS THAT WERE INTERPOLATED */
      for (auto& x: ctrlPoints)
      {
          glVertex2i(x.first, x.second);
      }
      
      glEnd();
      
      /* DRAW THE ONE POINT THE USER MADE */
      glColor3f(0.8, 0.75, 1);
      aitkenAlgSeg(points, univSize, u);
      
      break;
    case HERM:
      //cout << "viewpoint is HERM" << endl;
      glColor3f(0.8, 0.75, 1);
      glBegin(GL_POINTS);
      glColor3f(1, 1, 1);
      /* DRAW POINTS THAT WERE INTERPOLATED */
      for (auto& x: ctrlPoints)
      {
        glVertex2i(x.first, x.second);
      }
      
      glEnd();
      /* DRAW BUTTONS */
      
      /* BUTTON FOR AUTO */
      if(ctrlPoints2.size() < ctrlPoints.size())
      {
      glColor3f(0.25, 0.6, 1.0);
      glBegin(GL_QUADS);
      glVertex2i(135, -250);
      glVertex2i(135, -220);
      glVertex2i(180, -220);
      glVertex2i(180, -250);
      glEnd();
      glPushMatrix();
      glTranslatef(141, -240, 0);
      glScalef(0.1, 0.1, 0.1);
      glColor3f(1, 1, 1);
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'U');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'T');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'O');
      glPopMatrix();
      /* BUTTON FOR MANUAL */
      glColor3f(0.25, 0.6, 1.0);
      glBegin(GL_QUADS);
      glVertex2i(190, -250);
      glVertex2i(190, -220);
      glVertex2i(275, -220);
      glVertex2i(275, -250);
      glEnd();
      glPushMatrix();
      glTranslatef(205, -240, 0);
      glScalef(0.1, 0.1, 0.1);
      glColor3f(1, 1, 1);
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'M');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'N');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'U');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, 'L');
      glPopMatrix();
      }
      else
      {
        drawArrows();
        activePt = ctrlPoints2.begin();
        activePt2 = ctrlPoints2.begin();
        activePt2++;
        i = 0;
        for(auto& a: ctrlPoints)
        {
          points[i].x = a.first;
          points[i].y = a.second;
          i++;
        }
        // I'm going to wikipedia this bitch.
        
        //cout << "i'm here" << endl;
        glColor3f(0.5, 0.0, 1.0);
        
        //glLineWidth(2);
        //glPointSize(1.f);
        glEnd();
        glBegin(GL_POINTS);
        //glLineWidth(2);
        //glBegin(GL_LINES);
        //glEnable(GL_LINE_SMOOTH);
        
        
        for(i = 0; i < univSize - 1; i++)
        {
        for(float t = 0; t <= 1.0; t += 0.01)
        {
          int x = (2*t*t*t - 3*t*t + 1)*points[i].x + (t*t*t - 2*t*t + t)*activePt->first + (-2*t*t*t + 3*t*t)*points[i+1].x + (t*t*t - t*t)*activePt2->first;
          int y = (2*t*t*t - 3*t*t + 1)*points[i].y + (t*t*t - 2*t*t + t)*activePt->second + (-2*t*t*t + 3*t*t)*points[i+1].y + (t*t*t - t*t)*activePt2->second;
          //cout << " x is " << x << " and y is " << y << endl;
          glVertex2f(x, y);
          if(t != 0.0 || t != 1.0)
          glVertex2f(x, y);
        }
          activePt2++;
          activePt++;
        }
        glEnd();
        //glLineWidth(1.f);
      }
      instructions("To adjust points, hit the EDIT button.");
      
      
      break;
    case VECEN:
      glBegin(GL_POINTS);
      glColor3f(1, 0, 0);
      if(ctrlPoints2.size() < ctrlPoints.size())
      glVertex2f(activePt->first, activePt->second);
      glEnd();
      drawArrows();
      instructions("Click to set the vector of the active point.");
      break;
    case BERN:
      /*univSize = (int)ctrlPoints.size();
      Point bezPoints[univSize]; // = new Point[univSize];*/
      univSize = (int)ctrlPoints.size();
      slider();
      
      glColor3f(1, 1, 1);
      connectingLines();
      i = 0;
      for (auto& a: ctrlPoints)
      {
        points[i].x = a.first;
        points[i].y = a.second;
        i++;
      }
      
      GLint bezCurvePts = 10000;
      glBegin(GL_POINTS);
      glPointSize(1);
      glColor3f(0, 0, 1);
      bezier(points, univSize, bezCurvePts); // calculating 1000 bezCurve points
      glEnd();
      instructions("Move left or right keys to change degree.");
      break;
  }
  
  glFlush();
}

void degUp()
{
  univSize = (int)ctrlPoints.size();
  Point *oldpoints = new Point[univSize];
  ctrlPoints2.clear();
  
  int i = 0;
  for (auto& a: ctrlPoints)
  {
    oldpoints[i].x = a.first;
    oldpoints[i].y = a.second;
    //cout << i << ": " << "( " << a.first << ", " << a.second << ")" << endl;
    i++;
  }
  //cout << "****" << endl;
  ctrlPoints2.emplace_back(make_pair(oldpoints[0].x, oldpoints[0].y));
  
  for(i = 1; i < univSize; i++)
  {
    float med = ((float)i)/(univSize + 1);
    int x, y;
    x = nearbyint(med*oldpoints[i - 1].x + (1 - med)*oldpoints[i].x);
    y = nearbyint(med*oldpoints[i - 1].y + (1 - med)*oldpoints[i].y);
    ctrlPoints2.emplace_back(make_pair(x, y));
  }
  
  ctrlPoints2.emplace_back(make_pair(oldpoints[univSize - 1].x, oldpoints[univSize - 1].y));
  
  ctrlPoints = ctrlPoints2;
  
}

void degDown()
{
  int n = univSize - 2;
  MatrixXd m(n + 2, n + 1);
  m.setZero();
  m(0,0) = 1;

  
  /* GOES FROM ROW 1 TO THE SECOND TO LAST ROW */
  for(int i = 1; i < n + 1; i++)
  {
    m(i, i -1) = ((float)i)/(n + 1);
    m(i, i) = 1 - ((float)i)/(n + 1);
  }
  m(n + 1, n) = 1;

  MatrixXd bX(n+2, 1);
  MatrixXd bY(n+2, 1);
  
  int i = 0;
  for(auto& a: ctrlPoints)
  {
    bX(i, 0) = a.first;
    bY(i, 0) = a.second;
    i++;
  }
 
  
  MatrixXd solX = (m.transpose() * m).jacobiSvd(ComputeThinU | ComputeThinV).solve(m.transpose() * bX);
  MatrixXd solY = (m.transpose() * m).jacobiSvd(ComputeThinU | ComputeThinV).solve(m.transpose() * bY);
  
  solX(0, 0) = bX(0, 0);
  solY(0, 0) = bY(0, 0);
  
  solX(n, 0) = bX(n+1, 0);
  solY(n, 0) = bY(n+1, 0);
  
  ctrlPoints.clear();
  
  for(i = 0; i < n + 1; i++)
  {
    ctrlPoints.emplace_back(make_pair(solX(i, 0), solY(i, 0)));
  }

}

void addToListCurve(Point points[], int size, float t, Point clist1[], Point clist2[])
{
  if(size == 1)
  {
    //glColor3f(0, 0, 1);
    //glVertex2f(points[0].x, points[0].y); *something else
    //cout << points[0].x << " " << points[0].y;
    clist1[univSize-1].x = points[0].x;
    clist1[univSize-1].y = points[0].y;
    clist2[0].x = points[0].x;
    clist2[0].y = points[0].y;
    //clist2.emplace_front(points[0].x, points[0].y);
  }
  else
  {
    size--;
    Point newpoints[size];
    for(int i = 0; i < size; i++)
    {
      int x = (1-t)*points[i].x + t*points[i+1].x;
      int y = (1-t)*points[i].y + t*points[i+1].y;
      newpoints[i].x = x;
      newpoints[i].y = y;
    }
    drawCurve(newpoints, size, t);
    // glEnd();
  }
}
void drawCurve(Point points[], int size, float t)
{
  if(size == 1)
  {
    //glColor3f(0, 0, 1);
    glVertex2f(points[0].x, points[0].y);
    //cout << "IN DRAW CURVE " << points[0].x << ", " << points[0].y << endl;
    //clist1->emplace_back(make_pair(points[0].x, points[0].y));
  }
  else
  {
    size--;
    Point newpoints[size];
    for(int i = 0; i < size; i++)
    {
      int x = (1-t)*points[i].x + t*points[i+1].x;
      int y = (1-t)*points[i].y + t*points[i+1].y;
      newpoints[i].x = x;
      newpoints[i].y = y;
    }
    drawCurve(newpoints, size, t);
   // glEnd();
  }
}


void drawCurveSeg(Point points[], int size, float t)
{
  if(size == 1)
  {
    glColor3f(0, 0, 1);
    glEnd();
    glBegin(GL_POINTS);
    glVertex2f(points[0].x, points[0].y);
    //cout << "IN DRAW CURVESEG " << points[0].x << ", " << points[0].y << endl;
    glEnd();
  }
  else
  {
    glBegin(GL_LINES);
    size--;
    Point newpoints[size];
    for(int i = 0; i < size; i++)
    {
      int x = (1-t)*points[i].x + t*points[i+1].x;
      int y = (1-t)*points[i].y + t*points[i+1].y;
      newpoints[i].x = x;
      newpoints[i].y = y;
      glVertex2f(x,y);
      if(i != 0 && i != size - 1)
      {
        glVertex2f(x, y);
      }
      //drawCurve(newpoints, size, t);

    }
    drawCurveSeg(newpoints, size, t);
    glEnd();
  }
}

void aitkenAlg(Point points[], int n, float t)
{
  
 // for(int i = 0; i < n; i++)
  //  cout << ts[i] << " ";
  
 // cout << endl << endl;
  for(int r = 1; r < n; r++)
  {
    int l = n - r;
    for(int i = 0; i < l; i++)
    {
      float med = (ts[i + r] - t)/(ts[i+r] - ts[i]);
      //cout << points[i].x << endl;
      points[i].x = med*points[i].x + (1-med)*points[i+1].x;
      points[i].y = med*points[i].y + (1-med)*points[i+1].y;
    }
  }
 // cout << "at " << t << ": (" << points[0].x << ", " << points[0].y << ")" << endl;
  glVertex2f(points[0].x, points[0].y);
}

void aitkenAlgN(Point points[], int n, float t)
{
  
  // for(int i = 0; i < n; i++)
  //  cout << ts[i] << " ";
  
  // cout << endl << endl;
  for(int r = 1; r < n; r++)
  {
    int l = n - r;
    for(int i = 0; i < l; i++)
    {
      float med = (ts[i + r] - t)/(ts[i+r] - ts[i]);
      //cout << points[i].x << endl;
      points[i].x = med*points[i].x + (1-med)*points[i+1].x;
      points[i].y = med*points[i].y + (1-med)*points[i+1].y;
    }
  }
  // cout << "at " << t << ": (" << points[0].x << ", " << points[0].y << ")" << endl;
  univPoint.x = points[0].x;
  univPoint.y = points[0].y;
}

void aitkenAlgSeg(Point points[], int n, float t)
{
  
  // for(int i = 0; i < n; i++)
  //  cout << ts[i] << " ";

  connectingLines();
  glBegin(GL_LINES);
  cout << endl << endl;
  //glVertex2f(points[0].x, points[0].y);
   //cout << " (" << points[0].x << ", " << points[0].y << ")" << endl;
  for(int r = 1; r < n; r++)
  {
    int l = n - r;
    //glVertex2f(points[r].x, points[r].y);
    for(int i = 0; i < l; i++)
    {
      float med = (ts[i + r] - t)/(ts[i+r] - ts[i]);
      //cout << points[i].x << endl;
      glVertex2f(points[i].x, points[i].y);
      points[i].x = med*points[i].x + (1-med)*points[i+1].x;
      points[i].y = med*points[i].y + (1-med)*points[i+1].y;
        glVertex2f(points[i].x, points[i].y);
      glVertex2f(points[i].x, points[i].y);
      
      glVertex2f(points[i+1].x, points[i+1].y);
    //  if(!(r == n - 1 || i == n - r - 1))
     // glVertex2f(points[i].x, points[i].y);
      
    }
    //if(r != n - 1)
    //glVertex2f(points[r].x, points[r].y);
   // glVertex2f(points[r].x, points[r].y);
   // cout << " (" << points[r].x << ", " << points[r].y << ")" << endl;
  }
  // cout << "at " << t << ": (" << points[0].x << ", " << points[0].y << ")" << endl;
  glEnd();
  glBegin(GL_POINTS);
  glColor3f(0, 0, 1);
  glVertex2f(points[0].x, points[0].y);
  glEnd();
}

void pascalTriangle (GLint n, GLint * C)
{
  GLint k, j;
  
  for (k = 0;  k <= n;  k++) {
    //  Compute n!/(k!(n - k)!).
    C [k] = 1;
    for (j = n;  j >= k + 1;  j--)
      C [k] *= j;
    for (j = n - k;  j >= 2;  j--)
      C [k] /= j;
  }
}

void bezPointComp (GLfloat j, Point * bezPt, GLint nCtrlPts,
                   Point * bctrlPts, GLint * C)
{
  GLint k, n = nCtrlPts - 1;
  GLfloat bezBlendFcn;

  
  bezPt->x = bezPt->y = 0.0;
  
  //  Compute blending functions and blend control points.
  for (k = 0; k < nCtrlPts; k++) {
    bezBlendFcn = C [k] * pow (j, k) * pow (1 - j, n - k);
    bezPt->x += bctrlPts[k].x * bezBlendFcn;
    bezPt->y += bctrlPts[k].y * bezBlendFcn;
    //bezPt->z += ctrlPts [k].z * bezBlendFcn;
  }
}


void bezier (Point* bctrlPts, GLint nCtrlPts, GLint nBezCurvePts)
{
//  cout << nCtrlPts << " is the # of control points rn." << endl << endl; change for C1 continuity
  Point bezCurvePt;
  GLfloat t;
  GLint *C, k;
  
  //  Allocate space for binomial coefficients
  C = new GLint [nCtrlPts];
    pascalTriangle (nCtrlPts - 1, C);
  for (k = 0;  k <= nBezCurvePts;  k++) {
    t = GLfloat (k) / GLfloat (nBezCurvePts);
    bezPointComp (t, &bezCurvePt, nCtrlPts, bctrlPts, C);
   // glBegin (GL_POINTS);
    glVertex2f (bezCurvePt.x, bezCurvePt.y);
    //glEnd ( );
  }
  delete [ ] C;
}


void instructions(char *string)
{
  glPushMatrix();
  glTranslatef(-265, -250, 0);
  //glTranslatef(-250, 0);
  glScalef(0.1, 0.1, 0.1);
  glColor3f(1, 1, 1);
  for(int i = 0; string[i] != '\0'; i++)
  {
    glutStrokeCharacter(GLUT_STROKE_ROMAN, string[i]);
  }
  glPopMatrix();
}

void slider()
{
  univSize = (int)ctrlPoints.size();
  glPushMatrix();
  glColor3f(1, 1, 1);
  glTranslatef(45, -245, 0);
  glBegin(GL_LINES);
  glVertex2f(0, 0);
  glVertex2f(216, 0);
  for(int i = 0; i <= 216; i+=24)
  {
    glVertex2f(i, 5);
    glVertex2f(i, -5);
  }
  
  glEnd();
  glPopMatrix();
  glPushMatrix();
  glTranslatef(-20, -273, 0);
  for(int i = 3; i < 13; i++)
  {
    glPushMatrix();
    glScalef(0.1, 0.1, 0.1);
    if(i < 6)
      glTranslatef((200 + (i-3)*10)*i, 0, 0);
    else
    glTranslatef(227*i, 0, 0);
   // glTranslatef(400+(i-3)*200, -653, 0);
    if(i < 10)
    glutStrokeCharacter(GLUT_STROKE_ROMAN, (i+48));
    else
    {
      glutStrokeCharacter(GLUT_STROKE_ROMAN, '1');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, (i%10+48));
    }
    glPopMatrix();
  }
  glPopMatrix();
  
  /* PRINT THE CURRENT DEGREE */
  glPushMatrix();
  glColor3f(0, 0, 1);
  glTranslatef(-24, -245, 0);
  if(2 < univSize)
  {
    glBegin(GL_QUADS);
    glVertex2f(24*univSize-5, 5);
    glVertex2f(24*univSize+5, 5);
    glVertex2f(24*univSize+5, -5);
    glVertex2f(24*univSize-5, -5);
    glEnd();
  }
  glPopMatrix();
}

void atSlider()
{
  float n = ((int)ctrlPoints.size() - 1);
  glPushMatrix();
  glColor3f(1, 1, 1);
  glTranslatef(45, -245, 0);
  glBegin(GL_LINES);
  glVertex2f(0, 0);
  glVertex2f(216, 0);
  float val = ((float)216)/n;
  for(float i = 0; i <= 216; i+=val)
  {
    glVertex2f(i, 5);
    glVertex2f(i, -5);
  }
  glEnd();
  glPopMatrix();
  glPushMatrix();
  //glScalef(0.1, 0.1, 0.1);
  glTranslatef(300, -15, 0);
    instructions("0.0");
  glTranslatef(216, 0, 0);
   instructions("1.0");
//  sprintf(tmp1, "%0.3f", u);
//  strcat(tmp2, tmp1);
//  glPushMatrix();
//  glTranslatef(0, 40, 0);
//  instructions(tmp2);
  glPopMatrix();
  glPushMatrix();
  glColor3f(0, 0, 1);
  glPointSize(5);
  glBegin(GL_POINTS);
  glVertex2f(45 + 216*u, -245);
  glEnd();
  glTranslatef(43+216*u, -230, 0);
  glScalef(0.1, 0.1, 0.1);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 't');
  glPopMatrix();
}

void menu()
{
  /* CREATE MENU */
  //EDIT OPTION
  glPushMatrix();
  glTranslatef(-15, 20, 0);
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glVertex2i(-270, 240);
  glVertex2i(-270, 270);
  glVertex2i(-220, 270);
  glVertex2i(-220, 240);
  glEnd();
  glPushMatrix();
  glTranslatef(-258, 250, 0);
  //glTranslatef(-250, 0);
  glScalef(0.1, 0.1, 0.1);
  glColor3f(0.25, 0.6, 1.0);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'E');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'D');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'I');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'T');
  glPopMatrix();
  //deAITKENjau Algorithm
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glVertex2i(-210, 240);
  glVertex2i(-210, 270);
  glVertex2i(-135, 270);
  glVertex2i(-135, 240);
  glEnd();
  glPushMatrix();
  glTranslatef(-200, 250, 0);
  //glTranslatef(-250, 0);
  glScalef(0.1, 0.1, 0.1);
  glColor3f(0.25, 0.6, 1.0);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'P');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'R');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'T');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
  glPopMatrix();
  //Bern-Bezier Algorithm
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glVertex2i(-125, 240);
  glVertex2i(-125, 270);
  glVertex2i(-50, 270);
  glVertex2i(-50, 240);
  glEnd();
  glPushMatrix();
  glTranslatef(-115, 250, 0);
  glScalef(0.1, 0.1, 0.1);
  glColor3f(0.25, 0.6, 1.0);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'P');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'R');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'T');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'B');
  glPopMatrix();
  //Subdivide
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glVertex2i(-40, 240);
  glVertex2i(-40, 270);
  glVertex2i(35, 270);
  glVertex2i(35, 240);
  glEnd();
  glPushMatrix();
  glTranslatef(-30, 250, 0);
  glScalef(0.1, 0.1, 0.1);
  glColor3f(0.25, 0.6, 1.0);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'P');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'A');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'R');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'T');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, ' ');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'C');
  glPopMatrix();
  glPopMatrix();
  
} //Creates menu.
void drawArrows()
{
  int n = (int)ctrlPoints2.size();
  list< pair<int,int> >::iterator tmp = ctrlPoints.begin();
  list< pair<int,int> >::iterator tmp2 = ctrlPoints2.begin();
  for(int i = 0; i < n; i++)
  {
    drawArrow(tmp->first, tmp->second, tmp->first + tmp2->first, tmp->second + tmp2->second);
    tmp++;
    tmp2++;
  }
}

void drawArrow(int x1, int y1, int destX, int destY)
{
  glBegin(GL_LINES);
  glColor3f(0.4, 0, 1.0);
  
  glVertex2f(x1, y1);
  glVertex2f(destX, destY);
  
  glEnd();
  glBegin(GL_POINTS);
  glVertex2f(destX, destY);
  glEnd();
}

void keyInput(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 27:
      exit(0);
      break;
    default:
	     break;
  }
}

void specialKeyInput(int key, int x, int y)
{
  univSize = (int)ctrlPoints.size();
    if(viewpoint == BERN)
    {
      if(key == GLUT_KEY_LEFT)
      {
       // viewpoint=DEG;
        if(univSize > 3)
        degDown();
      }
      if(key == GLUT_KEY_RIGHT)
      {
        //viewpoint=DEG;
        if(univSize < 12)
          degUp();
      }
    }
  if(viewpoint == AITKEN)
  {
    if(key == GLUT_KEY_LEFT)
    {
      if(u >= 0.02)
      u -= 0.02;
    }
    if(key == GLUT_KEY_RIGHT)
    {
      if(u <= 0.98)
        u += 0.02;
    }
  }
  
  glutPostRedisplay();
}

void autoGenVec()
{
  activePt = ctrlPoints.begin();
  list< pair<int,int> >::iterator activePt2 = ctrlPoints.begin();
  
  activePt2++;
  
  while(activePt != ctrlPoints.end())
  {
    ctrlPoints2.emplace_back(make_pair((activePt2->first - activePt->first)/0.5, (activePt2->second - activePt->second)/0.5));
    activePt++;
    activePt2++;
  }
  
  //ctrlPoints2.emplace_back(make_pair(x - activePt->first, y - activePt->second));
  //activePt++;
} //automatically generates vectors.

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
  glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-300.0, 300.0, -300.0, 300.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
