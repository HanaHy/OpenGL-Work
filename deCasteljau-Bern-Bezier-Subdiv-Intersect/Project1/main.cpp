//
//  main.cpp
//  Project1
//
//  Created by Hana Hyder on 4/10/16.
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

using namespace std;

list< pair<int,int> > ctrlPoints;
list< pair<int,int> > ctrlPoints2;
list< pair<int,int> >::iterator activePt;
//float **castelVals;
int univSize;
bool activate;
bool compute = false;
bool nextCurve = false;
int threshold = 90000;
static float u = 0.5;
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
class BBOX
{
  bool isVertical;
public:
  Point LT1, LB1, RT1, RB1;
  BBOX(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
  {
    LT1.x = x1;
    LT1.y = y1;
    
    RT1.x = x2;
    RT1.y = y2;
    
    LB1.x = x3;
    LB1.y = y3;
    
    RB1.x = x4;
    RB1.y = y4;
    
    if(RT1.y - RB1.y > RB1.x - LB1.x)
    {
      isVertical = true;
    }
    else
      isVertical = false;
    
  }
  BBOX(Point topLeft, Point topRight, Point bottomLeft, Point bottomRight)
  {
    LT1.x = topLeft.x;
    LT1.y = topLeft.y;
    
    RT1.x = topRight.x;
    RT1.y = topRight.y;
    
    LB1.x = bottomLeft.x;
    LB1.y = bottomLeft.y;
    
    RB1.x = bottomRight.x;
    RB1.y = bottomRight.y;
    
    if(RT1.y - RB1.y > RB1.x - LB1.x)
    {
      isVertical = true;
    }
    else
      isVertical = false;
  }
  BBOX* split() {
    Point M1, M2;

    if(isVertical)
    {
      M1.x = LT1.x; //(LT1.x - LB1.x)/2 + LB1.x;
      M1.y = (LT1.y - LB1.y)/2 + LB1.y;
      M2.x = RT1.x;
      M2.y = (RT1.y - RB1.y)/2 + RB1.y;
    }
    else {
      M1.x = (RT1.x - LT1.x)/2 + LT1.x;
      M1.y = RT1.y; //(RT1.y - LT1.y)/2 + LT1.y;
      M2.x = (RB1.x - LB1.x)/2 + LB1.x;
      M2.y = RB1.y; //(RB1.y - LB1.y)/2 + LB1.y;
    }
    
    //if isVertical - cut horizontally (change
    if(isVertical) {
      BBOX *boxA = new BBOX(LT1, RT1, M1, M2);
      
     // LT1.x = M1.x;
      LT1.y = M1.y;
      
      //RT1.x = M2.x;
      RT1.y = M2.y;
      
      return boxA;
    }
    BBOX *boxB = new BBOX(M1, RT1, M2, RB1);
    
    RT1.x = M1.x;
   // RT1.y = M1.y;
    RB1.x = M2.x;
   // RB1.y = M2.y;
    
    return boxB;
   // return boxB;
  } // Changes original and returns a new box.
  void draw()
  {
    glBegin(GL_LINES);
    //glColor3f(0, 0, 1);
    glVertex2f(LT1.x, LT1.y);
    glVertex2f(RT1.x, RT1.y);
    
    glVertex2f(LB1.x, LB1.y);
    glVertex2f(RB1.x, RB1.y);
    
    glVertex2f(LT1.x, LT1.y);
    glVertex2f(LB1.x, LB1.y);
    
    glVertex2f(RT1.x, RT1.y);
    glVertex2f(RB1.x, RB1.y);
    
    glEnd();
  }
  float area()
  {
    return ((RT1.x - LT1.x)*(LT1.y - LB1.y));
  }
  
};
bool intersect(BBOX *RectA, BBOX *RectB)
{
  if (RectA->LT1.x < RectB->RT1.x && RectA->RT1.x > RectB->LT1.x &&
      RectA->LB1.y < RectB->LT1.y && RectA->LT1.y > RectB->LB1.y)
    return true;
  
  return false;
}
bool bezInt(BBOX *box1, BBOX *box2)
{
  if(intersect(box1, box2) == false)
  {
    return false;
  }
  if(box1->area() + box2->area() <= threshold)
  {
    glColor3f(1, 0, 0);
    box1->draw();
    glColor3f(0, 0, 1);
    box2->draw();
    return true;
  }
  BBOX *box1a = box1->split();
  BBOX *box2a = box2->split();
  
  
   bezInt(box1a, box2a);
  bezInt(box1a, box2);
  bezInt(box1, box2a);
  bezInt(box1, box2);
  return bezInt(box1a, box2a) || bezInt(box1a, box2) || bezInt(box1, box2a) || bezInt(box1, box2);
}

//Point *clist1;
//Point *clist2;

/*DISPLAYING*/
void display();
void menu();
void instructions(char *string);

/*PROCESSING*/
void processMouseClick(int button, int state, int x, int y);
void winReshapeFcn (GLint newWidth, GLint newHeight);
void keyInput(unsigned char key, int x, int y);
void specialKeyInput(int key, int x, int y);

/*CALCULATIONS*/
void linInterp();
void drawCurveSeg(Point points[], int size, float t);
void drawCurve(Point points[], int size, float t);
void addToListCurve(Point points[], int size, float t, Point clist1[], Point clist2[]);
void pascalTriangle(GLint n, GLint *C);
void bezier (Point* ctrlPts, GLint nCtrlPts, GLint nBezCurvePts);
void findExtrema(Point* ctrlPts, GLint nCtrlPts, GLint nBezCurvePts, short isFirst);

enum {
  SETPTS,ADJPTS,CASTEL,BERN,SUB,INTER
}viewpoint=SETPTS;

int main(int argc, char *argv[])
{
  activate = false;
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition (50, 50);
  glutInitWindowSize (winWidth, winHeight);
  
  xmin1 = ymin1 = xmin2 = ymin2 = xwcMax;
  xmax1 = ymax1 = xmax2 = ymax2 = xwcMin;
  
 glutCreateWindow("Project 1 Curves");
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
              viewpoint=CASTEL;
              break;
            }
            else if(x <= -23 && x >= -110)
            {
              viewpoint=BERN;
              break;
            }
            else if(x <= 70 && x >= -13)
            {
              viewpoint=SUB;
              break;
            }
            else if(x <= 160 && x >= 80)
            {
              viewpoint=INTER;
              nextCurve = false;
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
              viewpoint=CASTEL;
              break;
            }
            else if(x <= -23 && x >= -110)
            {
              viewpoint=BERN;
              break;
            }
            else if(x <= 70 && x >= -13)
            {
              viewpoint=SUB;
              break;
            }
            else if(x <= 160 && x >= 80)
            {
              viewpoint=INTER;
              nextCurve = false;
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
      
    case CASTEL:
    case BERN:
    case SUB:
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
          viewpoint=CASTEL;
          break;
        }
        else if(x <= -23 && x >= -110)
        {
          viewpoint=BERN;
          break;
        }
        else if(x <= 70 && x >= -13)
        {
          viewpoint=SUB;
          break;
        }
        else if(x <= 160 && x >= 80)
        {
          viewpoint=INTER;
          nextCurve = false;
          break;
        }

      }
      break;
    case INTER:
      if(button == GLUT_LEFT_BUTTON)
      {
        
        if(state == GLUT_DOWN)
        {
          if(x <= 265 && x >= 220 && y <= -220 && y >= -250)
          {
            if(nextCurve == true)
              compute = true;
            nextCurve = true;
            //cout << "it is true now" << endl;
          }
          if(x <= -220 && x>= -270 && y <= 270 && y >= 240)
          {
            viewpoint=ADJPTS;
            break;
          }
          if(y < 240 && y > -210) {
            if(!compute){
              if(!nextCurve) {
              ctrlPoints.emplace_back(make_pair(x, y));
              }
              else
              {
                ctrlPoints2.emplace_back(make_pair(x,y));
              }
            }
          }
          else if(y >= 240)
          {
            if(x <= -135 && x >= -225)
            {
              viewpoint=CASTEL;
              break;
            }
            else if(x <= -23 && x >= -110)
            {
              viewpoint=BERN;
              break;
            }
            else if(x <= 70 && x >= -13)
            {
              viewpoint=SUB;
              break;
            }
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
  if(viewpoint==INTER)
    glColor3f(1,0,1);
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
if(viewpoint==INTER)
  for (auto& x: ctrlPoints2)
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

  
  glEnd();
  univSize = (int)ctrlPoints.size();
  int size = univSize;
  int size2 = univSize;
  if(viewpoint==INTER)
    size2 = (int)ctrlPoints2.size();
  Point points[univSize]; // = new Point[univSize];
  Point clist1[size];
  Point clist2[size2];
  char tmp1[32];
  char tmp2[64] = "t = ";
  int i = 0;
  clock_t start;
  double duration;
  int k1, k2;
  
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
      
    case CASTEL:
      connectingLines();
      
      i = 0;
      for (auto& a: ctrlPoints)
      {
        points[i].x = a.first;
        points[i].y = a.second;
        i++;
      }
      
      /*for(i = 0; i < univSize; i++)
      {
        cout << "(" << points[i].x << ", " << points[i].y << "), ";
      }*/
      
      cout << endl << endl;
      
      glColor3f(0.2, 0.2, 1.0);
      glLineWidth(2);
      glBegin(GL_LINES);
      //glLineWidth(2.0);
      glEnable(GL_LINE_SMOOTH);
      for(float i = 0.0; i <= 1.0; i+= 0.004)
      {
        if(i != 0.0 && i != 1.0)
        {
          drawCurve(points, univSize, i);
        }
        drawCurve(points, univSize, i);
      }
      glDisable(GL_LINE_SMOOTH);
      glEnd();
      
      glLineWidth(1);
      
      glColor3f(0.25, 0.6, 1.0);
      //glBegin(GL_POINTS);
      drawCurveSeg(points, univSize, u);
      
      instructions("PRESS LEFT RIGHT KEYS TO CHANGE t.");
      sprintf(tmp1, "%0.3f", u);
      strcat(tmp2, tmp1);
      glPushMatrix();
      glTranslatef(0, 40, 0);
      instructions(tmp2);
      glPopMatrix();
      break;
    case SUB:
      glClear(GL_COLOR_BUFFER_BIT);
      menu();
      i = 0;
      univSize = (int)ctrlPoints.size();
      for (auto& a: ctrlPoints)
      {
        points[i].x = a.first;
        points[i].y = a.second;
        i++;
      }
      
      k1 = 0;
      k2 = 0;
      size = univSize;
      
      while(size != 1)
      {
          size--;
          for(i = 0; i < size; i++)
          {
              if(i == 0)
              {
                clist1[k1].x = points[i].x;
                clist1[k1].y = points[i].y;
                k1++;
              }
            if(i == size - 1)
            {
              clist2[k2].x = points[i+1].x;
              clist2[k2].y = points[i+1].y;
              k2++;
            }
            points[i].x = (1-u)*points[i].x + u*points[i+1].x;
            points[i].y = (1-u)*points[i].y + u*points[i+1].y;
          
          }
          if(size == 1)
          {
            clist1[univSize-1].x = points[0].x;
            clist1[univSize-1].y = points[0].y;
            clist2[univSize-1].x = points[0].x;
            clist2[univSize-1].y = points[0].y;
          }

          
        }
      clist2[0].x = points[univSize -1].x;
      clist2[0].y = points[univSize -1].y;
      glBegin(GL_POINTS);
      glPointSize(5);
      glColor3f(1, 0, 1);
      bezier(clist1, univSize, 250);
      //drawCurve(clist1, univSize, u);
      glColor3f(0, 0, 1);
      bezier(clist2, univSize, 250);
      //drawCurve(clist2, univSize, u);
      glEnd();
      glLineWidth(1);
      glBegin(GL_LINES);
      for(i = 0; i < univSize; i++)
      {
        if(i != 0 && i != univSize-1)
        {
          glVertex2f(clist2[i].x, clist2[i].y);
        }
        glVertex2f(clist2[i].x, clist2[i].y);
      }
      glColor3f(1, 0, 1);
      for(i = 0; i < univSize; i++)
      {
        if(i != 0 && i != univSize-1)
        {
          glVertex2f(clist1[i].x, clist1[i].y);
        }
        glVertex2f(clist1[i].x, clist1[i].y);
      }

      glEnd();
      
      glBegin(GL_POINTS);
      glPointSize(10);
      for(i = 0; i < univSize; i++)
      {
        glColor3f(1, 0, 1);
          glVertex2f(clist1[i].x, clist1[i].y);
        glColor3f(0, 0, 1);
        glVertex2f(clist2[i].x, clist2[i].y);
      }
      glEnd();
      instructions("PRESS LEFT RIGHT KEYS TO CHANGE t.");
      sprintf(tmp1, "%0.3f", u);
      strcat(tmp2, tmp1);
      glPushMatrix();
      glTranslatef(0, 40, 0);
      instructions(tmp2);
      glPopMatrix();

      break;
    case INTER:
      if(!compute){
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
      glColor3f(0, 1, 1);
      instructions("CLICK TO CREATE POINTS.");
      }
      else
      {
        i = 0;
        for (auto& a: ctrlPoints)
        {
          clist1[i].x = a.first;
          clist1[i].y = a.second;
          i++;
        }
        i = 0;
        for (auto& a: ctrlPoints2)
        {
          clist2[i].x = a.first;
          clist2[i].y = a.second;
          i++;
        }
        glBegin(GL_POINTS);
        glPointSize(1);
        glColor3f(1, 0, 1);
        findExtrema(clist1, size, 250, 1);
        glColor3f(0.8, 0.8, 0.8);
        findExtrema(clist2, size2, 250, 0);
        glEnd();
        glColor3f(0, 0, 1);
        BBOX *box1 = new BBOX(xmin1, ymax1, xmax1, ymax1, xmin1, ymin1, xmax1, ymin1);
       // box1->draw();
        //cout << "1: " << xmin1 << ", " << ymin1 << ", " << xmax1 << ", " << ymax1 << endl;
        glColor3f(1, 0, 0);
        BBOX *box2 = new BBOX(xmin2, ymax2, xmax2, ymax2, xmin2, ymin2, xmax2, ymin2);
       // box2->draw();
        //cout << "2: " << xmin2 << ", " << ymin2 << ", " << xmax2 << ", " << ymax2 << endl;
        //cout << box1->intersect(box1, box2) << endl;
        bezInt(box1, box2);
        strcpy(tmp2, "Area Threshold Value: ");
        instructions("PRESS UP DOWN KEYS TO CHANGE THE THRESHOLD.");
        sprintf(tmp1, "%i", threshold);
        strcat(tmp2, tmp1);
        glPushMatrix();
        glTranslatef(0, 40, 0);
        instructions(tmp2);
        glPopMatrix();
        
      }
      break;
    case BERN:
      /*univSize = (int)ctrlPoints.size();
      Point bezPoints[univSize]; // = new Point[univSize];*/
      univSize = (int)ctrlPoints.size();
      
      glColor3f(1, 1, 1);
      connectingLines();
      i = 0;
      for (auto& a: ctrlPoints)
      {
        points[i].x = a.first;
        points[i].y = a.second;
        i++;
      }
      
      GLint bezCurvePts = 2000;
      glBegin(GL_POINTS);
      glPointSize(1);
      glColor3f(0, 0, 1);
      bezier(points, univSize, bezCurvePts); // calculating 1000 bezCurve points
      glEnd();
      
      start = clock();
      bezier(points, univSize, bezCurvePts);
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      sprintf(tmp1, "%f s", duration);
      strcpy(tmp2, "Bern: ");
      strcat(tmp2, tmp1);
      glPushMatrix();
      glTranslatef(0, 40, 0);
      instructions(tmp2);
      glPopMatrix();
      
      start = clock();
     // glBegin(GL_LINES);
      for(float i = 0.0; i <= 1.0; i+= 0.0005)
      {
        if(i != 0.0 && i != 1.0)
        {
          drawCurve(points, univSize, i);
        }
        drawCurve(points, univSize, i);
      }
    //  glEnd();
      duration = (clock() - start)/(double)CLOCKS_PER_SEC;
      sprintf(tmp1, "%f s", duration);
      strcpy(tmp2, "Cast: ");
      strcat(tmp2, tmp1);
      glPushMatrix();
      glTranslatef(200, 40, 0);
      instructions(tmp2);
      glPopMatrix();
      instructions("NUMBER OF POINTS: 250");
      break;
  }
  
  glFlush();
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

void findExtrema(Point* bctrlPts, GLint nCtrlPts, GLint nBezCurvePts, short isFirst)
{
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
    if(isFirst == 1)
    {
      if(bezCurvePt.x < xmin1)
        xmin1 = bezCurvePt.x;
      
      if(bezCurvePt.y < ymin1)
        ymin1 = bezCurvePt.y;
      
      if(bezCurvePt.x > xmax1)
        xmax1 = bezCurvePt.x;
      
      if(bezCurvePt.y > ymax1)
        ymax1 = bezCurvePt.y;
      
    }
    else
    {
      if(bezCurvePt.x < xmin2)
        xmin2 = bezCurvePt.x;
      
      if(bezCurvePt.y < ymin2)
        ymin2 = bezCurvePt.y;
      
      if(bezCurvePt.x > xmax2)
        xmax2 = bezCurvePt.x;
      
      if(bezCurvePt.y > ymax2)
        ymax2 = bezCurvePt.y;
    }
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
  //deCasteljau Algorithm
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glVertex2i(-210, 240);
  glVertex2i(-210, 270);
  glVertex2i(-120, 270);
  glVertex2i(-120, 240);
  glEnd();
  glPushMatrix();
  glTranslatef(-200, 250, 0);
  //glTranslatef(-250, 0);
  glScalef(0.1, 0.1, 0.1);
  glColor3f(0.25, 0.6, 1.0);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'd');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'e');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'C');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'a');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 's');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 't');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'e');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'l');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'j');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'a');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'u');
  glPopMatrix();
  //Bern-Bezier Algorithm
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glVertex2i(-110, 240);
  glVertex2i(-110, 270);
  glVertex2i(-23, 270);
  glVertex2i(-23, 240);
  glEnd();
  glPushMatrix();
  glTranslatef(-102, 250, 0);
  glScalef(0.1, 0.1, 0.1);
  glColor3f(0.25, 0.6, 1.0);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'B');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'e');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'r');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'n');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, '-');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'B');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'e');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'z');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'i');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'e');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'r');
  glPopMatrix();
  //Subdivide
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glVertex2i(-13, 240);
  glVertex2i(-13, 270);
  glVertex2i(70, 270);
  glVertex2i(70, 240);
  glEnd();
  glPushMatrix();
  glTranslatef(-3, 250, 0);
  glScalef(0.1, 0.1, 0.1);
  glColor3f(0.25, 0.6, 1.0);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'S');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'U');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'B');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'D');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'I');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'V');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'I');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'D');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'E');
  glPopMatrix();
  //Intersect
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_QUADS);
  glVertex2i(80, 240);
  glVertex2i(80, 270);
  glVertex2i(160, 270);
  glVertex2i(160, 240);
  glEnd();
  glPushMatrix();
  glTranslatef(87, 250, 0);
  glScalef(0.1, 0.1, 0.1);
  glColor3f(0.25, 0.6, 1.0);
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'I');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'N');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'T');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'E');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'R');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'S');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'E');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'C');
  glutStrokeCharacter(GLUT_STROKE_ROMAN, 'T');
  glPopMatrix();
  glPopMatrix();
  
} //Creates menu.

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
  if(key == GLUT_KEY_LEFT)
    if ( u >= 0.004 ) u-= 0.004;
  if(key == GLUT_KEY_RIGHT)
    if ( u <= 0.996 ) u += 0.004;
  
  if(key == GLUT_KEY_DOWN)
    if(threshold >= 550)
      threshold -= 500;
  if(key == GLUT_KEY_UP)
    if(threshold <= 359500)
      threshold += 500;
  
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
