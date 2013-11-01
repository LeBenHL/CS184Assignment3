
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
#include "lodepng.h"


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

//The vertices that define our polygon
vector<vector<pair<ThreeDVector*, ThreeDVector*> > > polygons;

//Save Boolean
bool save = false;
//Filename
static const char* file_name;

//Print Function for debugging
void print(string _string) {
  cout << _string << endl;
}

//Create PNG Function
void createPng(const char* filename, std::vector<unsigned char>& image, unsigned width, unsigned height)
{
  //Encode the image
  unsigned error = lodepng::encode(filename, image, width, height);

  //if there's an error, display it
  if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
}

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
// reshape viewport if the window is resized
//****************************************************
void myReshape(int w, int h) {
  viewport.w = w;
  viewport.h = h;

  glViewport (0,0,viewport.w,viewport.h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(-5, 5, -5, 5, 5, -5);

}

//****************************************************
// Simple init function
//****************************************************
void initScene(){
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // Clear to black, fully transparent

  // Enable lighting and the light we have set up
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  //Set lighting parameters
  GLfloat light_position0[] = {1.0, 0.0, -1.0, 0};
  GLfloat light_ambient0[] = {0, 0, 0, 1};
  GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1};
  GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1};

  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);

  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);

  //Set Material Parameters
  GLfloat ambient_color[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat diffuse_color[] = { 0.0, 0.3, 0.3, 1.0 };
  GLfloat specular_color[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat shininess[] = { 2.0 };
  GLfloat emission[] = {0, 0, 0, 1};

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable shading
  glShadeModel(GL_SMOOTH);

  myReshape(viewport.w,viewport.h);
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer

  glMatrixMode(GL_MODELVIEW);			    // indicate we are specifying camera transformations
  glLoadIdentity();				            // make sure transformation is "zero'd"

  // Start drawing
  for(vector<vector<pair<ThreeDVector*, ThreeDVector*> > >::iterator it = polygons.begin(); it != polygons.end(); ++it) {
    vector<pair<ThreeDVector*, ThreeDVector*> > polygon = *it;
    glBegin(GL_POLYGON);                      // Draw A Polygon
    for(vector<pair<ThreeDVector*, ThreeDVector*> >::iterator i = polygon.begin(); i != polygon.end(); ++i) {
      pair<ThreeDVector*, ThreeDVector*> vertex_pair = *i;
      ThreeDVector* normal = vertex_pair.first;
      ThreeDVector* vertex = vertex_pair.second;
      glNormal3f(normal->x, normal->y, normal->z);
      glVertex3f(vertex->x, vertex->y, vertex->z);
    }
    glEnd();
  }
  glutSolidSphere(5.0, 100, 100);

  if (save) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    vector<unsigned char> buf(w * h * 4);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &buf[0] );

    createPng(file_name, buf, w, h);
  }

  glFlush();
  glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}



//****************************************************
// the usual stuff, nothing exciting here
//****************************************************

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
    } else if (string(argv[i]) == "-u") {
      adaptive = false;
    } else if (string(argv[i]) == "-save") {
      if(i + 1 < argc){
        save = true;
        file_name = argv[i + 1];
        i = i + 1;
      }
    }
  }

  parseBez(bez_file);

  if (adaptive) {
    cout << "Adaptive" << endl;
  } else {
    cout << "Uniform" << endl;
  }

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels 
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

  vector<pair<ThreeDVector*, ThreeDVector*> > polygon;
  polygon.push_back(make_pair(new ThreeDVector(0, 0, -1), new ThreeDVector(-1.0, 1.0, 0)));
  polygon.push_back(make_pair(new ThreeDVector(0, 0, -1), new ThreeDVector(1.0, 1.0, 0)));
  polygon.push_back(make_pair(new ThreeDVector(0, 0, 1), new ThreeDVector(1.0, -1.0, 0)));
  polygon.push_back(make_pair(new ThreeDVector(0, 0, 1), new ThreeDVector(-1.0, -1.0, 0)));
  polygons.push_back(polygon);

  vector<pair<ThreeDVector*, ThreeDVector*> > polygon2;
  polygon2.push_back(make_pair(new ThreeDVector(0, 0, -1), new ThreeDVector(-3.0, -1.0, 0)));
  polygon2.push_back(make_pair(new ThreeDVector(0, 0, -1), new ThreeDVector(-1.0, -1.0, 0)));
  polygon2.push_back(make_pair(new ThreeDVector(0, 0, -1), new ThreeDVector(-1.0, -3.0, 0)));
  polygon2.push_back(make_pair(new ThreeDVector(0, 0, -1), new ThreeDVector(-3.0, -3.0, 0)));
  polygons.push_back(polygon2);

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








