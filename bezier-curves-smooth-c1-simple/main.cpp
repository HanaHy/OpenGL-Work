//
//  main.cpp
//  HW1
//
//  Created by Hana Hyder on 1/10/16.
//  Copyright © 2016 UCD. All rights reserved.
//

#include <iostream>
#include <fstream>
/* Remove for submission */

#include <math.h>

#define PI 3.14159265
 #include <GL/glut.h> // unmark for submission
#include <stdlib.h>
#include <math.h>
using namespace std;

/*  Set initial size of the display window.  */
GLsizei winWidth = 600, winHeight = 600;

/*  Set size of world-coordinate clipping window.  */
GLfloat xwcMin = -300.0, xwcMax = 300.0;
GLfloat ywcMin = -300.0, ywcMax = 300.0;

void processMouseClick(int button, int state, int x, int y); // The call back function
void writeToFile();

class wcPt3D {
public:
  GLfloat x, y, z;
};

void init (void)
{
  /*  Set color of display window to white.  */
  glClearColor (1.0, 1.0, 1.0, 0.0);
}

void plotPoint (wcPt3D bezCurvePt)
{
  glBegin (GL_POINTS);
  glVertex2f (bezCurvePt.x, bezCurvePt.y);
  glEnd ( );
}

class BezCurve {
	public:
	int nSize;
	wcPt3D *ctrlPoints;
  	BezCurve()
	{
		nSize = 0;
		
	}
	BezCurve(wcPt3D *ptr, int size) {
		nSize = size;
		ctrlPoints = ptr;
		
	}
};

BezCurve *bezCurves;
int currentPoint, currentCurve, nCurves, numPts;
/*  Compute binomial coefficients C for given value of n.  */
void binomialCoeffs (GLint n, GLint * C)
{
  GLint k, j;
  
  for (k = 0;  k <= n;  k++) {
    /*  Compute n!/(k!(n - k)!).  */
    C [k] = 1;
    for (j = n;  j >= k + 1;  j--)
      C [k] *= j;
    for (j = n - k;  j >= 2;  j--)
      C [k] /= j;
  }
}

void computeBezPt (GLfloat u, wcPt3D * bezPt, GLint nCtrlPts,
                   wcPt3D * ctrlPts, GLint * C)
{
  GLint k, n = nCtrlPts - 1;
  GLfloat bezBlendFcn;
  
  bezPt->x = bezPt->y = bezPt->z = 0.0;
  
  /*  Compute blending functions and blend control points. */
  for (k = 0; k < nCtrlPts; k++) {
    bezBlendFcn = C [k] * pow (u, k) * pow (1 - u, n - k);
    bezPt->x += ctrlPts [k].x * bezBlendFcn;
    bezPt->y += ctrlPts [k].y * bezBlendFcn;
    bezPt->z += ctrlPts [k].z * bezBlendFcn;
  }
}

void bezier (wcPt3D * ctrlPts, GLint nCtrlPts, GLint nBezCurvePts)
{
  wcPt3D bezCurvePt;
  GLfloat u;
  GLint *C, k;
  
  /*  Allocate space for binomial coefficients  */
  C = new GLint [nCtrlPts];
  
  binomialCoeffs (nCtrlPts - 1, C);
  for (k = 0;  k <= nBezCurvePts;  k++) {
    u = GLfloat (k) / GLfloat (nBezCurvePts);
    computeBezPt (u, &bezCurvePt, nCtrlPts, ctrlPts, C);
    plotPoint (bezCurvePt);
  }
  delete [ ] C;
}

void displayFcn (void)
{
  /*  Set example number of control points and number of
   *  curve positions to be plotted along the Bezier curve.
   */
  //glClear(GL_COLOR_BUFFER_BIT);
  GLint nBezCurvePts = 1500;
  
  //wcPt3D ctrlPts [7] = { {-40.0, -40.0, 0.0}, {-10.0, 10.0, 0.0}, {0.0, -100.0, 0.0},
   // {10.0, 100.0, 0.0}, {20.0, -100.0, 0.0}, {30.0, 50.0, 0.0}, {40, -100.0, 0.0} };
  
  glClear (GL_COLOR_BUFFER_BIT);   //  Clear display window.
  
  for(int j = 0;  j < nCurves; j++) {
  //glPointSize (6);
  glColor3f ((double)rand()/RAND_MAX, (double)rand()/RAND_MAX, (double)rand()/RAND_MAX);      //  Set point color to red.
  
  //bezier (ctrlPts, nCtrlPts, nBezCurvePts);
  glPointSize(2);
  bezier(bezCurves[j].ctrlPoints, bezCurves[j].nSize, nBezCurvePts);
  glColor3f(0.0, 0.2, (double)rand()/RAND_MAX);
 glPointSize(6); 
 glBegin(GL_POINTS);
	for(int i = 0; i < bezCurves[j].nSize; i++)
	{
		glVertex2f(bezCurves[j].ctrlPoints[i].x, bezCurves[j].ctrlPoints[i].y);
	}
  glEnd();
	glPointSize(1);
glBegin(GL_LINES);
	for(int i = 0; i < bezCurves[j].nSize; i++) {
		glVertex2f(bezCurves[j].ctrlPoints[i].x, bezCurves[j].ctrlPoints[i].y);
		if(i > 0 && i < bezCurves[j].nSize - 1) {
			glVertex2f(bezCurves[j].ctrlPoints[i].x, bezCurves[j].ctrlPoints[i].y);
		}
	}
glEnd();
}
  glFlush ( );
}

void winReshapeFcn (GLint newWidth, GLint newHeight)
{
  /*  Maintain an aspect ratio of 1.0.  */
  glViewport (0, 0, newHeight, newHeight);
  
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ( );
  
  gluOrtho2D (xwcMin, xwcMax, ywcMin, ywcMax);
  
  glClear (GL_COLOR_BUFFER_BIT);
}

int main (int argc, char** argv)
{
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
  glutInitWindowPosition (50, 50);
  glutInitWindowSize (winWidth, winHeight);
  glutCreateWindow ("Bezier Curve");
  
  init ( );
  std::ifstream inf(argv[1]);
 // int nCurves = 0;
  int tmpSize = 0;
  int x1, y1, z1;
  if(inf.is_open()) {
    inf >> nCurves;
    bezCurves = new BezCurve[nCurves];

    for(int j = 0; j < nCurves; j++) {
	
	    inf >> tmpSize;
	    wcPt3D *ptr = new wcPt3D[tmpSize];
		numPts = tmpSize;
	    for(int i = 0; i < tmpSize; i++)
	    {
		inf >> x1 >> y1 >> z1;
		ptr[i].x = x1;
		ptr[i].y = y1;
		ptr[i].z = z1;
    	    }
		

/*		int k;
                wcPt3D *temp = new wcPt3D;
            for(int i = 0; i < tmpSize; i++) {
                k = i;
                while(k > 0 && ptr[k].x < ptr[k - 1].x) {
			temp->x = ptr[k].x;
			temp->y = ptr[k].y;
			temp->z = ptr[k].z;

			ptr[k].x = ptr[k - 1].x;
			ptr[k].y = ptr[k - 1].y;
			ptr[k].z = ptr[k - 1].z;

			ptr[k - 1].x = temp->x;
			ptr[k - 1].y = temp->y;
			ptr[k - 1].z = temp->z;
			k--;
		}
        }*/

	    bezCurves[j].ctrlPoints = ptr;
	    bezCurves[j].nSize = tmpSize;

    }
    
  }

	

  std::cout << nCurves << std::endl;
   std::cout << tmpSize << std::endl;

   for(int i = 0; i < nCurves; i++) {
     std::cout << std::endl << "Curve #" << i << ": " << std::endl;
	for(int j = 0; j < bezCurves[i].nSize; j++)
	{
		std::cout << "(" << bezCurves[i].ctrlPoints[j].x << ", " << bezCurves[i].ctrlPoints[j].y << ", " << bezCurves[i].ctrlPoints[j].z << "), ";
	}
	std::cout << std::endl;
   }
  glutDisplayFunc (displayFcn);
  glutReshapeFunc (winReshapeFcn);
  glutMouseFunc(processMouseClick);
  atexit(writeToFile);
  glutMainLoop ( );
}

void processMouseClick(int button, int state, int x, int y)
{
	x = -1*(300 - x);
	y = 300 - y;
	if(button == GLUT_LEFT_BUTTON) {
		if(state == GLUT_DOWN) {
			currentPoint = -1;
			currentCurve = -1;
		for(int j = 0; j < nCurves; j++)
		for(int i = 0; i < bezCurves[0].nSize; i++)
		{
			if(abs(bezCurves[j].ctrlPoints[i].x - x) <= 3 && abs(bezCurves[j].ctrlPoints[i].y - y) <= 3)
			{
				currentPoint = i;
				currentCurve = j;
				break;
			}
		}
		}
		if(state == GLUT_UP) {
			if(currentPoint != -1) {
			bezCurves[currentCurve].ctrlPoints[currentPoint].x = x;
			bezCurves[currentCurve].ctrlPoints[currentPoint].y = y;
			if(currentCurve > 0 && currentPoint == 0) {
				bezCurves[currentCurve - 1].ctrlPoints[numPts - 1].x = x;
				bezCurves[currentCurve - 1].ctrlPoints[numPts - 1].y = y;
				
				int dx, dy;
				dx = x - bezCurves[currentCurve - 1].ctrlPoints[numPts - 2].x;
				dy = y - bezCurves[currentCurve - 1].ctrlPoints[numPts - 2].y;
				bezCurves[currentCurve].ctrlPoints[1].x = x + dx;
                        	bezCurves[currentCurve].ctrlPoints[1].y = y + dy;
			}
			if(currentPoint == numPts - 2 && currentCurve != nCurves - 1) { //2nd to last point moved
				//This means it needs to move the 2nd point of the next curve
				int dx, dy;
				dx = bezCurves[currentCurve + 1].ctrlPoints[0].x - x;
				dy = bezCurves[currentCurve + 1].ctrlPoints[0].y - y;
				bezCurves[currentCurve + 1].ctrlPoints[1].x = bezCurves[currentCurve + 1].ctrlPoints[0].x + dx;
				bezCurves[currentCurve + 1].ctrlPoints[1].y = bezCurves[currentCurve + 1].ctrlPoints[0].y + dy;
			}
			if(currentPoint == 1 && currentCurve != 0) { //2nd point moved
				//This means it needs to move the 2nd to last point of the previous curve
				//cout << "THIS CASE 3" << endl;
				int dx, dy;
				dx = bezCurves[currentCurve - 1].ctrlPoints[numPts - 1].x - x;
				dy = bezCurves[currentCurve - 1].ctrlPoints[numPts - 1].y - y;
				bezCurves[currentCurve - 1].ctrlPoints[numPts - 2].x = bezCurves[currentCurve - 1].ctrlPoints[numPts -1].x + dx;
				bezCurves[currentCurve - 1].ctrlPoints[numPts - 2].y = bezCurves[currentCurve - 1].ctrlPoints[numPts -1].y + dy;
			}
			displayFcn();
			}
		}

	}

}

void writeToFile() {
  ofstream outfile("output.txt");

  outfile << nCurves << std::endl;
  for(int i = 0; i < nCurves; i++) {
  outfile << numPts << std::endl;
	for(int j = 0; j < numPts; j++) {
		outfile << bezCurves[i].ctrlPoints[j].x << " " << bezCurves[i].ctrlPoints[j].y << " " << bezCurves[i].ctrlPoints[j].z << std::endl;
	}
	}
	outfile.close();

}
