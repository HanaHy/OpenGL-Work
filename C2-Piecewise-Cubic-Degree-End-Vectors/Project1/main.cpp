//
//  main.cpp
//  Project3
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
list< pair<int,int> > ctrlPoints3;
list< pair<int,int> > ctrlPoints4;
list< pair<int,int> >::iterator activePt;
float *ratios;


float *ts;

//float **castelVals;
int univSize;
bool activate;
bool createAuto;
static float u;
int knotType;
int univK;
float tminus1, tminus2, tnplus1;

/*  Set initial size of the display window.  */
GLsizei winWidth = 600, winHeight = 600;

/*  Set size of world-coordinate clipping window.  */
GLfloat xwcMin = -300.0, xwcMax = 300.0;
GLfloat ywcMin = -300.0, ywcMax = 300.0;

/* Class Point */
class Point
{
public:
  Point(){x=0.;y=0.;};
  Point operator=(const Point rhs) ;
  Point operator+(const Point rhs) const;
  Point operator*(float m) const;
  float x;
  float y;
};

Point Point::operator=(const Point rhs)
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

Point Point::operator+(const Point rhs) const
{
  Point temp;
  temp.x = x + rhs.x;
  temp.y = y + rhs.y;
  return temp;
}
Point Point::operator*(float m) const
{
  Point temp;
  temp.x = x*m;
  temp.y = y*m;
  return temp;
}


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
void entryMode();

/*CALCULATIONS*/
void pascalTriangle(GLint n, GLint *C);
void evalKnotSequence(Point points[]); //Depends on knotType value.

/* DRAWING */
void bezier (Point* ctrlPts, GLint nCtrlPts, GLint nBezCurvePts);
void c2CubicSpline(Point points[], int l);
Point deBoorAlg(Point points[], int k, int I);
void c1quadFunc(Point points[]);

enum {
  SETPTS,ADJPTS,AITKEN,BERN,HERM,VECEN,VECEN2
}viewpoint=SETPTS;

int main(int argc, char *argv[])
{
  u = 0;
  activate = false;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition (50, 50);
  glutInitWindowSize (winWidth, winHeight);
  
  
 glutCreateWindow("Project 3 Curves");
  glutMouseFunc(processMouseClick);
  //glClearColor(0.25, 0.75, 0.9, 1);
//  glClearColor(0.78, 0.64, 0.78, 1);
  glClearColor(107/255.0, 142/255.0, 35/255.0, 1);
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
              knotType=0;
              ctrlPoints2.clear();
              break;
            }
            else if(x <= -50 && x >= -125)
            {
              viewpoint=AITKEN;
              univK = 1;
              break;
            }
            else if(x >= -40 && x <= 35)
            {
              viewpoint=HERM;
              createAuto = true;
              knotType = 0;
              ctrlPoints4.clear();
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
              knotType=0;
              ctrlPoints2.clear();
              break;
            }
            else if(x <= -50 && x >= -125)
            {
              viewpoint=AITKEN;
              univK = 1;
              break;
            }
            else if(x >= -40 && x <= 35)
            {
              viewpoint=HERM;
              createAuto = true;
              knotType=1;
              ctrlPoints4.clear();
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
          knotType=0;
          ctrlPoints2.clear();
          break;
        }
        else if(x <= -50 && x >= -125)
        {
          viewpoint=AITKEN;
          univK = 1;
          break;
        }
        else if(x >= -40 && x <= 35)
        {
          viewpoint=HERM;
          createAuto = true;
          knotType=1;
          ctrlPoints4.clear();
        }
        if((int)ctrlPoints2.size() != 2)
          viewpoint=VECEN;break;

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
          knotType=0;
          ctrlPoints2.clear();
          break;
        }
        else if(x <= -50 && x >= -125)
        {
          viewpoint=AITKEN;
          univK = 1;
          break;
        }
        
                
      }
        if(ctrlPoints4.size() < 2)
        {
          //activePt = ctrlPoints.begin();
          //ctrlPoints3.clear();
          viewpoint = VECEN2;
          break;
        }
      break;
    case VECEN:
      if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
      if((int)ctrlPoints2.size() < 2)
      {
       // cout << "Click Pt  - x is: " << x << "and y is " << y << endl;
       // cout << "Active Pt - x is: " << activePt->first << "and y is " << activePt->second << endl;
       // cout << "Direc Vct - x is: " << x - activePt->first << "and y is " << y - activePt->second << endl;
        ctrlPoints2.emplace_back(make_pair(x, y));
        //activePt++;
        if((int)ctrlPoints2.size() == 2)
        {
          viewpoint = BERN;
          break;
        }
        
          }
          else
          {
            viewpoint = BERN;
          }
        }
      }
      
      break;
      
    case VECEN2:
      if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
          if((int)ctrlPoints4.size() < 2)
          {
            ctrlPoints4.emplace_back(make_pair(x, y));
            //activePt++;
            if((int)ctrlPoints4.size() == 2)
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

void evalKnotSequence(Point points[])
{
  
  /*
   knotType:
   0 - uniform distribution
   1 - chord length parameterization
   2 - centripetal parameterization
   */
  /*switch (knotType) {
    case 1:
      break;
    case 2:
      break;
      
    default:
    case 0:
      
      break;
  }
  */
  int size2 = univSize;
  int k = univK;
  int i = 0;
  if(viewpoint==AITKEN)
  {
    ts = new float[univSize + k]; // length of n + k + 1.
    for(int j = 0; j < univSize + k; j++)
    {
      ts[j] = ((float)j)/(univSize+k-1);
      //cout << ts[j] << " ";
    }
    //cout << endl << endl << "**********" << endl;
  }
  else if(viewpoint == BERN)
  {
    ts = new float[univSize+2]; //setting the size.
    if(knotType==0)
    {
    float tmpVal = 1.0;
    if(univSize != 0)
      tmpVal = ((float)1)/(univSize + 1);
      
     // tmpVal += 0.02;
      
    int p = 0;
    for(float incr = 0.0; incr <= 1.0; incr += tmpVal)
    {
      //ts[i] = incr;
      ts[p] = tmpVal;
      // cout << incr << " ";
      p++;
    } //uniform distrib.
    
    tminus1 = tmpVal;
    tminus2 = tmpVal;
    tnplus1 = tmpVal;
    }
    else if(knotType==1)
    {
      
      float L = 0;
      
      for(i = 0; i < univSize-1;i++ )
      {
        L = L + pow(pow(points[i+1].x - points[i].x,2.0)+pow(points[i+1].y - points[i].y,2.0),0.5);
      }
      
      
    /*  for(i = 1; i <= univSize-2;i++)
      {
        for(int j = 0; j < i; j++)
        {
          if(j+1 == i)
          {
            ts[i] = ts[i] + pow((pow(points[j+1].x - points[j].x,2.0) + pow(points[j+1].y-points[j].y,2.0)), 0.5);
          }
          else{
            ts[i] += ts[j];
          }
        }
      }*/
      
      for(int j = 0; i < univSize-1; i++)
      {
        ts[j] = pow((pow(points[j+1].x - points[j].x,2.0) + pow(points[j+1].y-points[j].y,2.0)), 0.5);
      }
      
      float tmpVal = 1.0;
        tmpVal = ((float)1)/((int)ctrlPoints.size() - 1);
      
     // tminus1 = tmpVal;
     // tminus2 = tmpVal;
     // tnplus1 = tmpVal;
      tminus1 = ts[0];
      tminus2 = ts[0];
      tnplus1 = ts[0];
    
      if(univSize != 0)
        tmpVal = ((float)1)/(univSize + 1);
      
      // tmpVal += 0.02;
      
      int p = 0;
      for(float incr = 0.0; incr <= 1.0; incr += tmpVal)
      {
        //ts[i] = incr;
        ts[p] = tmpVal;
        // cout << incr << " ";
        p++;
      } //uniform distrib.
      
      tminus1 = tmpVal;
      tminus2 = tmpVal;
      tnplus1 = tmpVal;

      
    }
    else
    {
      float tmpVal = 1.0;
      if(univSize != 0)
        tmpVal = ((float)1)/(univSize + 1);
      
      int p = 0;
      for(float incr = 0.0; incr <= 1.0; incr += tmpVal)
      {
        //ts[i] = incr;
        ts[p] = tmpVal;
        // cout << incr << " ";
        p++;
      } //uniform distrib.
      
      tminus1 = tmpVal;
      tminus2 = tmpVal;
      tnplus1 = tmpVal;
    }
  }
  else
  {
    if(knotType == 0)
    {
      ts = new float[size2]; //setting the size.
    
      float tmpVal = 1.0;
      if(size2 != 0)
        tmpVal = ((float)1)/((int)ctrlPoints.size() - 1);
      for(float incr = 0.0; incr <= 1.0; incr += tmpVal)
      {
        ts[i] = incr;
      //ts[i] = tmpVal;
      // cout << incr << " ";
        i++;
      } //uniform distrib.
    
      tminus1 = tmpVal;
      tminus2 = tmpVal;
      tnplus1 = tmpVal;
    }
    else if(knotType==1)
    {
      float L = 0;
      
      for(i = 0; i < univSize-1;i++ )
      {
        L = L + pow(pow(points[i+1].x - points[i].x,2.0)+pow(points[i+1].y - points[i].y,2.0),0.5);
      }
      
//      cout << "THE TOTAL LENGTH IS : " << L << endl << endl;
        i = 0;
      float tmpVal = 1.0;
      if(size2 != 0)
        tmpVal = ((float)1)/((int)ctrlPoints.size() - 1);
      
      for(i = 1; i <= univSize-2;i++)
      {
        for(int j = 0; j < i; j++)
        {
          if(j+1 == i)
          {
            ts[i] = ts[i] + pow((pow(points[j+1].x - points[j].x,2.0) + pow(points[j+1].y-points[j].y,2.0)), 0.5)/L;
          }
          else{
            ts[i] += ts[j];
          }
        }
      }
      
      
      /*for(float incr = 0.0; incr <= 1.0; incr += tmpVal)
      {
        ts[i] = incr;
        //ts[i] = tmpVal;
        // cout << incr << " ";
        i++;
      } //uniform distrib.
      */
      tminus1 = tmpVal;
      tminus2 = tmpVal;
      tnplus1 = tmpVal;
    }
    else if(knotType==2)
    {
      float tmpVal = 1.0;
      if(size2 != 0)
        tmpVal = ((float)1)/((int)ctrlPoints.size() - 1);
      for(float incr = 0.0; incr <= 1.0; incr += tmpVal)
      {
        ts[i] = incr;
        //ts[i] = tmpVal;
        // cout << incr << " ";
        i++;
      } //uniform distrib.
      
      tminus1 = tmpVal;
      tminus2 = tmpVal;
      tnplus1 = tmpVal;
    }
  }
  
}//Depends on knotType value.


void entryMode()
{
  createAuto = false;
  cout << "-------------IN ENTRY MODE-------------------" << endl;
  int tmpSize = (int)ctrlPoints.size()-2;
  ratios = new float[tmpSize];
  
  //ratios[0] = 0.0;
  int i = 0;
  while(i < tmpSize)
  {
    cout << "For " << i+1 << ": ";
    cin >> ratios[i];
    i++;
  }
  //ratios[tmpSize-1] = 1.0;
  
  glutPostRedisplay();
  
}

//main display loop, this function will be called by OpenGL
void display()
{
  //Misc.
  glClear(GL_COLOR_BUFFER_BIT);
  menu(); //Creates menu.
  
  glColor3f(1,1,1);
  glPointSize(5);
  
  glBegin(GL_POINTS);
  bool tmp = true;
  
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
    {
      if(viewpoint==AITKEN)
        glColor3f(60/255.0, 179/255.0, 113/255.0);
      
      glVertex2i(x.first, x.second);
      
    }
    tmp = true;
  }
  glColor3f(1, 1, 1);
  
  glEnd();
  univSize = (int)ctrlPoints.size();

  const int size2 = univSize;
  Point *points = new Point[size2];
 
  char tmp1[32];
  char tmp2[64] = "u = ";
  int i = 0;
  
  i = 0;
  for (auto& a: ctrlPoints)
  {
    points[i].x = a.first;
    points[i].y = a.second;
    i++;
  }
  
  int I = -1;
  int k = univK;
  
  float uTEMP;
  bool trust;
  list< pair<int,int> >::iterator activePt2;
/*  if(viewpoint==AITKEN)
  {
    ts = new float[univSize + k]; // length of n + k + 1.
    for(int j = 0; j < univSize + k; j++)
    {
      ts[j] = ((float)j)/(univSize+k-1);
      //cout << ts[j] << " ";
    }
    //cout << endl << endl << "**********" << endl;
  }
  else
  {
    ts = new float[size2]; //setting the size.
  
  float tmpVal = 1.0;
  if(size2 != 0)
    tmpVal = ((float)1)/((int)ctrlPoints.size() - 1);
  for(float incr = 0.0; incr <= 1.0; incr += tmpVal)
  {
    ts[i] = incr;
    //ts[i] = tmpVal;
   // cout << incr << " ";
    i++;
  } //uniform distrib.

  tminus1 = tmpVal;
  tminus2 = tmpVal;
  tnplus1 = tmpVal;
  }*/
  
  evalKnotSequence(points);
  
  
  //tminus1, tminus2, tnplus1;
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
      glColor3f(60/255.0, 179/255.0, 113/255.0);
      connectingLines();
      
      i = 0;
      for (auto& a: ctrlPoints)
      {
        points[i].x = a.first;
        points[i].y = a.second;
        i++;
      }
      
     //instructions("ENTER THE DESIRED K VALUE.");
      //atSlider();
      slider();
//      cout << "============================================" << endl;
//      cout << "Enter the desired k value (-1 to stop): ";
//      cin >> u;
//      cout << endl;
//      cout << "============================================" << endl;
      
      instructions("L and R for k, Up and Down for u.");
      sprintf(tmp1, "%.3f", u);
      strcat(tmp2, tmp1);
      glPushMatrix();
      glTranslatef(0, 40, 0);
      instructions(tmp2);
      glPopMatrix();
      glBegin(GL_LINES);
      glVertex2i(-27, -238);
      glVertex2i(-17, -238);
      glVertex2i(-266, -198);
      glVertex2i(-256, -198);
      glEnd();
      
     /* glBegin(GL_POINTS);
      glPointSize(1);
      glColor3f(0, 128/255.0, 0);
      
      //float evalPt = ts[k-1];
      I = k-1;
      for(float evalPt = ts[k-1]; evalPt <= ts[univSize+1]; evalPt+=0.1)
      {
        deBoorAlgDraw(points, k, I, evalPt);
        if(ts[I+1] <= evalPt)
          I++;
      }
      
      glEnd();*/
      
      
      uTEMP = u;
      trust = false;
      glLineWidth(3);
      glEnable (GL_LINE_SMOOTH);
      glBegin(GL_LINES);
      glColor3f(0, 128/255.0, 0);
      for(u = 0.0; u < ts[univSize]; u += 0.01)
      {
      for(i = k-1; i <= univSize; i++)
      {
        if(u >= ts[i] && u < ts[i+1])
        {
          I = i;
          break;
        }
      }
      
      /* DEBOOR ALGORITHM DEFINED IN CLASS */
      
      
      if(I == -1)
      {
        //cout << "Invalid u value....press the up or down arrows." << endl;
      }
      else
      {
        //glBegin(GL_POINTS);
      //  glPointSize(1);
        //glColor3f(0, 128/255.0, 0);
        
        //glColor3f(0, 1, 0);
        
        deBoorAlg(points, k, I);
        
        if(trust)
        deBoorAlg(points, k, I);
        
        
        trust = true;
        //glEnd();
      }
      }
      
      glEnd();
       glLineWidth(1);
      glDisable(GL_LINE_SMOOTH);
      
      u = uTEMP;
      for(i = k-1; i <= univSize; i++)
      {
        if(u >= ts[i] && u < ts[i+1])
        {
          I = i;
          break;
        }
      }
      
      /* DEBOOR ALGORITHM DEFINED IN CLASS */
      
      if(u > ts[univSize] || u < ts[k-1])
        I = -1;
      
      
      if(I == -1)
      {
        cout << "Invalid u value....press the up or down arrows.\n\n\n\n\n" << endl;
      }
      else
      {
        glBegin(GL_POINTS);
        glPointSize(1);
        //glColor3f(0, 128/255.0, 0);
        
        glColor3f(0, 1, 0);
        deBoorAlg(points, k, I);
        glEnd();
      }
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
      if(ctrlPoints4.size() == 2)
      {
        drawArrows();
        i = 0;
        for(auto& a: ctrlPoints)
        {
          points[i].x = a.first;
          points[i].y = a.second;
          i++;
        }
        
        
        //cout << "i'm here" << endl;
        glColor3f(0.5, 0.0, 1.0);
        
        
        cout << "\n\n\n\n\n\n\n\n====================PART C MENU======================" << endl;
        cout << "Currently using a ";
          cout << "CHORD LENGTH knot sequence." << endl;
        
        cout << "Press: " << endl;
        cout << "  E or e : TO CHANGE SEQUENCE" << endl;
        //cout << "     2 : CENTRIPETAL PARAMETERIZATION KNOT SEQUENCE" << endl;
        
        cout << "=====================================================" << endl << endl;
        
        c1quadFunc(points);
      }
      instructions("To adjust points, hit the EDIT button.");
      
      
      break;
    case VECEN:
      glBegin(GL_POINTS);
      glColor3f(1, 0, 0);
      for(auto& a: ctrlPoints2)
      {
        glVertex2f(a.first, a.second);
      }
      glEnd();
      //drawArrows();
      instructions("Click to set the end point vectors.");
      break;
    case VECEN2:
      glBegin(GL_POINTS);
      glColor3f(1, 0, 0);
      for(auto& a: ctrlPoints4)
      {
        glVertex2f(a.first, a.second);
      }
      glEnd();
      //drawArrows();
      instructions("Click to set the end point vectors.");
      break;
    case BERN:
      cout << "\n\n\n\n\n\n\n\n====================PART A MENU======================" << endl;
      cout << "Currently using a ";
      if(knotType==0)
        cout << "UNIFORM knot sequence." << endl;
      if(knotType==1)
        cout << "CHORD LENGTH knot sequence." << endl;
      if(knotType==2)
        cout << "CENTRIPETAL PARAMETERIZATION knot sequence." << endl;
      
      cout << "Press: " << endl;
      cout << "     0 : UNIFORM KNOT SEQUENCE" << endl;
      cout << "     1 : CHORD LENGTH KNOT SEQUENCE" << endl;
      cout << "     2 : CENTRIPETAL PARAMETERIZATION KNOT SEQUENCE" << endl;
      
      cout << "=====================================================" << endl << endl;

      univSize = (int)ctrlPoints.size();
      //slider();
      
      if(ctrlPoints2.size() != 2)
      {
        viewpoint=VECEN;break;
      }
      
      glColor3f(1, 1, 1);
      connectingLines();
      i = 0;
      for (auto& a: ctrlPoints)
      {
        points[i].x = a.first;
        points[i].y = a.second;
        i++;
      }
      
      c2CubicSpline(points, univSize+1);
      break;
  }
  
  glFlush();
}


Point deBoorAlg(Point points[], const int k, const int I)
{
  Point **p = new Point*[k];
  //float **xVals = new float*[k-1];
  for(int i = 0; i < k; i++)
    p[i] = new Point[univSize];
  
  
  for(int i = 0; i < univSize; i++)
    p[0][i] = points[i];
  
  for(int r = 1; r <= k-1; r++)
  {
    for(int i= I-(k-1); i <= I- r; i++)
    {
      p[r][i] = p[r-1][i]*((ts[i+k] - u)/(ts[i+k]-ts[i+r])) + p[r-1][i+1]*((u - ts[i+r])/(ts[i+k]-ts[i+r]));
    }
  }
  
  glVertex2i(p[k-1][I-(k-1)].x, p[k-1][I-(k-1)].y);
  return p[k-1][I-(k-1)];
}

void c1quadFunc(Point points[])
{
  
  univSize = (int)ctrlPoints.size();
  Point *fpoints = new Point[univSize+(univSize-1)];
  
  for(int i = 0; i < univSize; i++)
  {
    fpoints[i*2] = points[i];
  }

  list< pair<int,int> >::iterator tmp1 = ctrlPoints4.begin();

  int vecX = (tmp1->first - points[0].x)/35.0;
  int vecY = (tmp1->second - points[0].y)/35.0;
  
  fpoints[1].x = points[0].x + (points[1].x - points[0].x)*1.0*vecX/2.0;
  fpoints[1].y = points[0].y + (points[1].y - points[0].y)*1.0*vecY/2.0;
  
  // for i = 1;
  cout << 0.0 << endl;
  for(int i = 1; i <= (univSize+(univSize-1)-2)/2; i++)
      {
        float ratio;
        if(createAuto)
        {
          ratio = pow(pow(fpoints[2*(i+1)].x - fpoints[2*i].x,2)+pow(fpoints[2*(i+1)].y - fpoints[2*i].y,2),0.5)/pow(pow(fpoints[2*i].x - fpoints[2*(i-1)].x,2)+pow(fpoints[2*i].y - fpoints[2*(i-1)].y,2),0.5);
        }
        else
        {
          ratio = ratios[i-1];
        }
        cout << ratio << endl;
        fpoints[2*i+1].x = ((fpoints[2*i].x - fpoints[2*i-1].x)*ratio) + fpoints[2*i].x;
        fpoints[2*i+1].y = (fpoints[2*i].y - fpoints[2*i-1].y)*ratio + fpoints[2*i].y;
      }
  cout << 1.0 << endl;
  glColor3f(0, 1, 1);
  glBegin(GL_LINES);
  
  for(int i = 0; i < univSize+ univSize-1; i++)
  {
    glVertex2i(fpoints[i].x, fpoints[i].y);
    if(i!= 0)
    glVertex2i(fpoints[i].x, fpoints[i].y);
  }
  
  
  glEnd();
  
  Point *newPoints = new Point[3];
  GLint bezCurvePts = 10000;
  glBegin(GL_POINTS);
  glPointSize(1);
  glColor3f(0, 128/255.0, 0);

  for(int i = 0; i < univSize + univSize-1; i++)
  {
    if(i%2 != 0 || i == 0)
    newPoints[i%2] = fpoints[i];
    
    if(i%2 == 0 && i != 0)
    {
      newPoints[2] = fpoints[i];
      bezier(newPoints, 3, bezCurvePts);
      newPoints[0] = fpoints[i];
    }
  }

  glEnd();
  glBegin(GL_POINTS);
  glColor3f(0, 1, 1);
  for(int i = 1; i < univSize+ univSize-1; i+=2)
  {
    glVertex2i(fpoints[i].x, fpoints[i].y);

  }
  
  glColor3f(1, 1, 1);
  for(int i = 0; i < univSize; i++)
    glVertex2i(points[i].x, points[i].y);
  
  glEnd();
  
  
}

void c2CubicSpline(Point points[], int l)
{
  univSize = (int)ctrlPoints.size(); // K = univSize - 1
  
  /*ts = new float[univSize+2]; //setting the size.
  float tmpVal = 1.0;
  if(univSize != 0)
    tmpVal = ((float)1)/(univSize + 1);
  int p = 0;
  for(float incr = 0.0; incr <= 1.0; incr += tmpVal)
  {
    //ts[i] = incr;
    ts[p] = tmpVal;
    // cout << incr << " ";
    p++;
  } //uniform distrib.

  tminus1 = tmpVal;
  tminus2 = tmpVal;
  tnplus1 = tmpVal;
*/
  
  evalKnotSequence(points);
  
  ctrlPoints3.clear();
  
  ctrlPoints3.emplace_back(make_pair(points[0].x, points[0].y));
  
  /* SET UP N MATRIX */
  int n = univSize - 1;
  MatrixXd m(n + 3, n + 3); // K - 1 x K - 1
  m.setZero();

  m(0,0) = 1;
  /* populating for i = 0 */
  m(1, 0) = ts[0]*ts[0]/(tminus2+ tminus1 + ts[0]);
  m(1, 1) = ts[0]*(tminus2 + tminus1)/(tminus2 + tminus1 + ts[0]) + tminus1*(ts[0] + ts[1])/(tminus1 + ts[0] + ts[1]);
  m(1, 2) = tminus1*tminus1/(tminus1+ts[0]+ts[1]);
  /* populating for i = 1 */
  m(2, 1) = ts[1]*ts[1]/(tminus1 + ts[0] + ts[1]);
  m(2, 2) = ts[1]*(tminus1 + ts[0])/(tminus1+ts[0] + ts[1]) + ts[0]*(ts[1] + ts[2])/(ts[0] + ts[1] + ts[2]);
  m(2, 3) = ts[0]*ts[0]/(ts[0] + ts[1] + ts[2]);
  m(n+2, n+2) = 1;
  for(int i = 2; i < n; i++)
  {
    m(i+1, i) = ts[i]*ts[i]/(ts[i-2]+ts[i-1] + ts[i]);
    m(i+1, i+1) = ts[i]*(ts[i-2] + ts[i-1])/(ts[i-2] + ts[i-1] + ts[i]) + ts[i-1]*(ts[i] + ts[i+1])/(ts[i-1]+ts[i] + ts[i+1]);
    m(i+1, i+2) = ts[i-1]*ts[i-1]/(ts[i-1]+ts[i]+ts[i+1]);
  }
  m(n+1, n) = ts[n]*ts[n]/(ts[n-2]+ts[n-1] + ts[n]);
  m(n+1, n+1) = ts[n]*(ts[n-2] + ts[n-1])/(ts[n-2] + ts[n-1] + ts[n]) + ts[n-1]*(ts[n] + tnplus1)/(ts[n-1]+ts[n] + tnplus1);
  m(n+1, n+2) = ts[n-1]*ts[n-1]/(ts[n-1]+ts[n]+tnplus1);
  
  /* SET UP POINTS MATRIX WITH WEIGHTAGES */
  
  MatrixXd bX(n+3, 1);
  MatrixXd bY(n+3, 1);
  
  int i = 0;
  for(auto& a: ctrlPoints)
  {
    if(i == 0)
    {
      bX(i+1, 0) = (tminus1 + ts[0])*a.first;
      bY(i+1, 0) = (tminus1 + ts[0])*a.second;
    }
    else
    {
      bX(i+1, 0) = (ts[i-1] + ts[i])*a.first;
      bY(i+1, 0) = (ts[i-1] + ts[i])*a.second;
    }
    i++;
  }
  
  int j = 0;
  glBegin(GL_POINTS);
  glColor3f(60/255.0, 179/255.0, 113/255.0);
  for(auto &a: ctrlPoints2)
  {
    glVertex2f(a.first, a.second);
//    cout << "(" << a.first << ", " << a.second << "), ";
    if(j == 0)
    {
      bX(0,0) = a.first;
      bY(0,0) = a.second;
    }
    else
    {
      bX(n+2, 0) = a.first;
      bY(n+2, 0) = a.second;
    }
    j++;
  }
  glEnd();

  
  MatrixXd solX = (m).jacobiSvd(ComputeThinU | ComputeThinV).solve(bX);
  MatrixXd solY = (m).jacobiSvd(ComputeThinU | ComputeThinV).solve(bY);
  
  
  Point *newPoints = new Point[n+3];
  for(int i = 0; i < n+3; i++)
  {
    newPoints[i].x = solX(i, 0);
    newPoints[i].y = solY(i, 0);
  }
  
  glBegin(GL_LINES);
  glColor3f(60/255.0, 179/255.0, 113/255.0);
  for(int i = 0; i < n + 3; i++)
  {
    glVertex2f(newPoints[i].x, newPoints[i].y);
    if(i != 0 && i != n + 2)
      glVertex2f(newPoints[i].x, newPoints[i].y);
  }
  glEnd();
  glBegin(GL_LINES);
  bool tmp = false;
  int k = 1;
  for(int i = 0; i < n + 2; i++)
  {
    if(i != 0 && i != n + 2)
    {
      glVertex2f(newPoints[i].x+ (ts[i-1]/(ts[i-1] + ts[i] + ts[i+1]))*(newPoints[i+1].x - newPoints[i].x), newPoints[i].y+ (ts[i-1]/(ts[i-1] + ts[i] + ts[i+1]))*(newPoints[i+1].y - newPoints[i].y)); // 1/3
      //if(tmp)
      if(i != n + 1)
        ctrlPoints3.emplace_back(make_pair(newPoints[i].x+ (ts[i-1]/(ts[i-1] + ts[i] + ts[i+1]))*(newPoints[i+1].x - newPoints[i].x), newPoints[i].y+ (ts[i-1]/(ts[i-1] + ts[i] + ts[i+1]))*(newPoints[i+1].y - newPoints[i].y))); // 1/3
    }
    
    
    if(i == 0)
    {
      glVertex2f(newPoints[i].x+ ((ts[i]+ts[i+1])/(tminus1+ts[i]+ts[i+1]))*(newPoints[i+1].x - newPoints[i].x), newPoints[i].y+ ((ts[i]+ts[i+1])/(tminus1+ts[i]+ts[i+1]))*(newPoints[i+1].y - newPoints[i].y)); // 2/3
    }
    else
    {
      glVertex2f(newPoints[i].x+ ((ts[i]+ts[i+1])/(ts[i-1]+ts[i]+ts[i+1]))*(newPoints[i+1].x - newPoints[i].x), newPoints[i].y+ ((ts[i]+ts[i+1])/(ts[i-1]+ts[i]+ts[i+1]))*(newPoints[i+1].y - newPoints[i].y)); // 2/3
    }
    
    if(tmp && i != n + 1)
    {
    ctrlPoints3.emplace_back(make_pair(newPoints[i].x+ ((ts[i]+ts[i+1])/(tminus1+ts[i]+ts[i+1]))*(newPoints[i+1].x - newPoints[i].x), newPoints[i].y+ ((ts[i]+ts[i+1])/(tminus1+ts[i]+ts[i+1]))*(newPoints[i+1].y - newPoints[i].y))); // 2/3
      
      if(k < univSize)
      {
        ctrlPoints3.emplace_back(make_pair(points[k].x, points[k].y));
        ctrlPoints3.emplace_back(make_pair(points[k].x, points[k].y));
      }
      k++;
    }
    tmp = true;
  }
  
  glEnd();
  
  
  delete [] newPoints;
  
  newPoints = new Point[4];

  
  GLint bezCurvePts = 10000;
  glBegin(GL_POINTS);
  glPointSize(1);
  glColor3f(0, 128/255.0, 0);
  int count = 1;
  for(auto& a : ctrlPoints3)
  {
    if(count % 4 == 0)
    {
      newPoints[3].x = a.first;
      newPoints[3].y = a.second;
      bezier(newPoints, 4, bezCurvePts);
    }
    else
    {
      newPoints[count % 4 - 1].x = a.first;
      newPoints[count % 4 - 1].y = a.second;
    }
    count++;
  }
  glEnd();
  
  glBegin(GL_POINTS);
  glColor3f(0, 0.5, 1);
  
  for(auto& a: ctrlPoints3)
  {
    glVertex2f(a.first, a.second);
  }
  
  glEnd();
  
  ctrlPoints3.clear();

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
  
  //Go from 1 to n+1
  univSize = (int)ctrlPoints.size();
  glPushMatrix();
  glColor3f(1, 1, 1);
  glTranslatef(45, -245, 0);
  glBegin(GL_LINES);
  glVertex2f(0, 0);
  glVertex2f(216, 0);
  int tmp = 216.0/(univSize-1);
  for(float i = 0; i < 216; i+=tmp)
  {
    glVertex2f(i, 5);
    glVertex2f(i, -5);
    
  }
  
  glVertex2f(216, 5);
  glVertex2f(216, -5);
  
  glEnd();
  glPopMatrix();
  glPushMatrix();
  glTranslatef(45, -273, 0);

  int j = 1;
  for(float i = 0; i <= 216; i+=tmp)
  {
    glPushMatrix();
    glTranslatef(i, 0, 0);
    glScalef(0.1, 0.1, 0.1);
    
    if(j < 10)
      glutStrokeCharacter(GLUT_STROKE_ROMAN, (j+48));
    else
    {
      glutStrokeCharacter(GLUT_STROKE_ROMAN, '1');
      glutStrokeCharacter(GLUT_STROKE_ROMAN, (j%10+48));
    }
    j++;
    glPopMatrix();
    
  }
  glPopMatrix();
  //cout << univK << endl;
  
  /* PRINT THE CURRENT DEGREE */
  glPushMatrix();
  glColor3f(0, 0, 1);
  glTranslatef(45, -245, 0);
    glBegin(GL_QUADS);
    glVertex2f(tmp*(univK-1)-5, 5);
    glVertex2f(tmp*(univK-1)+5, 5);
    glVertex2f(tmp*(univK-1)+5, -5);
    glVertex2f(tmp*(univK-1)-5, -5);
    glEnd();
  glPopMatrix();
  glPushMatrix();
  glTranslatef(45+tmp*(univK-1), -230, 0);
  glScalef(0.1, 0.1, 0.1);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'k');
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
  if(viewpoint!=HERM)
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
  else
  {
    int n = (int)ctrlPoints.size();
    list< pair<int,int> >::iterator tmp = ctrlPoints.begin();
    list< pair<int,int> >::iterator tmp2 = ctrlPoints4.begin();
    for(int i = 0; i < n; i++)
    {
      if(i == 0 || i == n-1)
      {
        drawArrow(tmp->first, tmp->second, tmp2->first, tmp2->second);
        tmp2++;
      }
      tmp++;
    }
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
  if(viewpoint==BERN)
  {
    if(key=='0')
      knotType=0;
    
    if(key=='1')
      knotType=1;
    
    if(key=='2')
      knotType=2;
    
    glutPostRedisplay();
  }
  if(viewpoint==HERM)
  {
    if(key=='E' || key=='e')
    entryMode();
  }
}

void specialKeyInput(int key, int x, int y)
{
  univSize = (int)ctrlPoints.size();
    if(viewpoint == BERN)
    {
      if(key == GLUT_KEY_LEFT)
      {
       
      }
      if(key == GLUT_KEY_RIGHT)
      {
       
      }
    }
  if(viewpoint == AITKEN)
  {
    if(key == GLUT_KEY_DOWN)
    {
      if(u >= 0.02)
      u -= 0.02;
    }
    if(key == GLUT_KEY_UP)
    {
      if(u <= 0.98)
        u += 0.02;
    }
    if(key == GLUT_KEY_LEFT)
    {
      if(univK >= 2)
        univK--;
    }
    if(key == GLUT_KEY_RIGHT)
    {
      if(univK < univSize)
        univK++;
    }
  }
  
  glutPostRedisplay();
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
  glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-300.0, 300.0, -300.0, 300.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}
