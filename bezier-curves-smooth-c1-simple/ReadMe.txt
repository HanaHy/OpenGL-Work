Hello! This program constructs smooth C1-continuous bezier curves. The output of heart.txt input file is shown in the Screenshots.
The other screenshot shows the manipulation.


To compile the program:
  Just type "make" and enter

To run the program:
  After compiling, just type "./test [filename]" where filename is the desired
  input file

The way the input file is read:
  Line # | Number of Numbers | Meaning of Number(s)
    1    |        1          | Y number of Curves
    2    |        1          | X number of Points on Curve
X to X-2 |        3          | "x y z" of point
   (Line 2 and X to X - 2 format are repeated Y times)
   The input file just needs spaces between the x y z coordinates, but line breaks for subsequent points/curves

When exiting the program:
   The new control points for the bezier curve are stored in a new file
   called "output.txt"

Note: Another file, heart.txt can also be used for input! :-)

--- SOURCES USED ---
Used the Chap8ProgramsTxt >> "prog8BezierCurve.txt" file from the 41609-Hearn_Programs zip on SmartSite for generating the Bezier curves.

Also consulted with Aaron Lewis for file reading and Javier Perez for mouse clicks.

