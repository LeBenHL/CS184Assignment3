
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#include <time.h>
#include <math.h>

#include "three_d_vector.h"
#include "bez_surface.h"


#define PI 3.14159265  // Should be used from mathlib
#define KEY_S 115
#define KEY_W 119
#define KEY_PLUS 43
#define KEY_MINUS 45
#define KEY_H 104

inline float sqr(float x) { return x*x; }

using namespace std;
double subdivision_parameter;

//The file we are parsing
char* bez_file;

//How many surfaces the file contains
int num_surfaces;

//Current Bez surface we are building
BezSurface* surface = new BezSurface();

//Our Surfaces
vector<BezSurface*> surfaces;

//Whether or not we use adaptive/uniform subdivision (DEFAULT ADAPTIVE)
bool adaptive = true;

//****************************************************
// Some Classes
//****************************************************

class Viewport;

class Viewport {
  public:
    int w, h; // width and height
};


//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;




//****************************************************
// Simple init function
//****************************************************
void initScene(){

  // Nothing to do here for this simple example.

}


//****************************************************
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, viewport.w, 0, viewport.h);

}


//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************

void setPixel(int x, int y, GLfloat r, GLfloat g, GLfloat b) {
  glColor3f(r, g, b);
  glVertex2f(x + 0.5, y + 0.5);   // The 0.5 is to target pixel
  // centers 
  // Note: Need to check for gap
  // bug on inst machines.
}

//****************************************************
// Draw a filled circle.  
//****************************************************


void circle(float centerX, float centerY, float radius) {
  // Draw inner circle
  glBegin(GL_POINTS);

  // We could eliminate wasted work by only looping over the pixels
  // inside the sphere's radius.  But the example is more clear this
  // way.  In general drawing an object by loopig over the whole
  // screen is wasteful.

  int i,j;  // Pixel indices

  int minI = max(0,(int)floor(centerX-radius));
  int maxI = min(viewport.w-1,(int)ceil(centerX+radius));

  int minJ = max(0,(int)floor(centerY-radius));
  int maxJ = min(viewport.h-1,(int)ceil(centerY+radius));



  for (i=0;i<viewport.w;i++) {
    for (j=0;j<viewport.h;j++) {

      // Location of the center of pixel relative to center of sphere
      float x = (i+0.5-centerX);
      float y = (j+0.5-centerY);

      float dist = sqrt(sqr(x) + sqr(y));

      if (dist<=radius) {

        // This is the front-facing Z coordinate
        float z = sqrt(radius*radius-dist*dist);

        setPixel(i,j, 1.0, 0.0, 0.0);

        // This is amusing, but it assumes negative color values are treated reasonably.
        // setPixel(i,j, x/radius, y/radius, z/radius );
      }


    }
  }


  glEnd();
}
//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
  glLoadIdentity();				        // make sure transformation is "zero'd"


  // Start drawing
  circle(viewport.w / 2.0 , viewport.h / 2.0 , min(viewport.w, viewport.h) / 3.0);

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
void print(string _string) {
  cout << _string << endl;
}

void parseBez(const char* filename) {
  int line_count = 1;

  std::ifstream inpfile(filename);
  if(!inpfile.is_open()) {
    std::cout << "Unable to open file" << std::endl;
  } else {
    std::string line;
    //MatrixStack mst;
    
    while(inpfile.good()) {
      std::vector<std::string> splitline;
      std::string buf;

      std::getline(inpfile,line);
      std::stringstream ss(line);

      while (ss >> buf) {
        splitline.push_back(buf);
      }
      //Ignore blank lines
      if(splitline.size() == 0) {
        continue;
      }

      //First line. Tells us how many surfaces there are
      else if (line_count == 1) {
        num_surfaces = atoi(splitline[0].c_str());
        line_count++;
      //Control Point Parsing
      } else if(line_count > 1) {
        int curve_index = (line_count - 2) % 4;
        long double x1, x2, x3, x4, y1, y2, y3, y4, z1, z2, z3, z4;
        x1 = atof(splitline[0].c_str());
        y1 = atof(splitline[1].c_str());
        z1 = atof(splitline[2].c_str());
        ThreeDVector* c1 = new ThreeDVector(x1, y1, z1);

        x2 = atof(splitline[3].c_str());
        y2 = atof(splitline[4].c_str());
        z2 = atof(splitline[5].c_str());
        ThreeDVector* c2 = new ThreeDVector(x2, y2, z2);

        x3 = atof(splitline[6].c_str());
        y3 = atof(splitline[7].c_str());
        z3 = atof(splitline[8].c_str());
        ThreeDVector* c3 = new ThreeDVector(x3, y3, z3);

        x4 = atof(splitline[9].c_str());
        y4 = atof(splitline[10].c_str());
        z4 = atof(splitline[11].c_str());
        ThreeDVector* c4 = new ThreeDVector(x4, y4, z4);

        ThreeDVector* curve[4];
        curve[0] = c1;
        curve[1] = c2;
        curve[2] = c3;
        curve[3] = c4;
        surface->add_control_points(curve_index, curve);

        line_count++;

        if (curve_index == 3) {
          surfaces.push_back(surface);
          surface = new BezSurface();
        }
      }
    }
  }
}

void myKeyboardFunc(unsigned char key, int x, int y){
  switch(key){
    case KEY_S:
      print("Toggle");
      break;
    case KEY_W:
      print("Wireframe");
      break;
    case KEY_PLUS:
      print("Zoom In");
      break;
    case KEY_MINUS:
      print("Zoom Out");
      break;
    case KEY_H:
      print("Hidden Line");
      break;
    default:
      break;
  }
}

void mySpecialKeyFunc(int key, int x, int y){
  bool shift = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
  switch(key){
    case GLUT_KEY_UP:
      if (shift) {
        print("Translate Up");
      } else {
        print("Rotate Up");
      }
      break;
    case GLUT_KEY_DOWN:
      if (shift) {
          print("Translate Down");
        } else {
        print("Rotate Down");
      }
      break;
    case GLUT_KEY_LEFT:
      if (shift) {
        print("Translate Left");
      } else {
        print("Rotate Left");
      }
      break;
    case GLUT_KEY_RIGHT:
      if (shift) {
        print("Translate Right");
      } else {
        print("Rotate Right");
      }
      break;
    default:
      break;
  }
}

int main(int argc, char *argv[]) {

  if (argc < 3) {
    cout << "USAGE: ./as3 [bez file] [subdivision parameter] [flags]" << endl;
    exit(1);
  }

  bez_file = argv[1];
  subdivision_parameter = atof(argv[2]);


  for (int i = 3; i < argc; i++) {
    if (string(argv[i]) == "-a") {
      adaptive = true;
    } else {
      adaptive = false;
    }
  }

  parseBez(bez_file);
  cout << surfaces.size() << endl;

  if (adaptive) {
    cout << "Adaptive" << endl;
  } else {
    cout << "Uniform" << endl;
  }

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();							// quick function to set up scene

  glutDisplayFunc(myDisplay);				// function to run when its time to draw something
  glutReshapeFunc(myReshape);				// function to run when the window gets resized
  glutKeyboardFunc(myKeyboardFunc); // basic keys callback
  glutSpecialFunc(mySpecialKeyFunc); //special keys callback

  glutMainLoop();							// infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}








