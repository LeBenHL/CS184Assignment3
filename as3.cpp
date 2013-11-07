
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
#define KEY_P 112

#define scale_step 0.05
#define translate_step 0.05
#define rotation_step 1

inline float sqr(float x) { return x*x; }

using namespace std;
double subdivision_parameter;

//The file we are parsing
char* file_to_parse;

//How many surfaces the file contains
int num_surfaces;

//Current Bez surface we are building
BezSurface* surface = new BezSurface();

//Our Surfaces
vector<BezSurface*> surfaces;

//Whether or not we use adaptive/uniform subdivision (DEFAULT UNIFORM)
bool adaptive = false;

//The vertices that define our polygon
vector<vector<pair<ThreeDVector*, ThreeDVector*> > > polygons;

//Save Boolean
bool save = false;
//Filename
static const char* file_name;

//Scale Multipliers for Zoom
double scale_x = 1;
double scale_y = 1;
double scale_z = 1;

//Translate values
double translate_x = 0;
double translate_y = 0;
double translate_z = 0;

//Rotation in degrees
double rotate_x = 0;
double rotate_y = 0;
double rotate_z = 0;

//bool for wireframe
bool wireframe = false;

//bool for flat shading
bool flat = false;

//Counter for printing
int counter = 0;

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

void parseObj(const char* filename) {
  cout << "Parsing Object File" << endl;
  
  vector<ThreeDVector*> vertices;
  vector<ThreeDVector*> vertices_normals;

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
      //Valid commands:
      //v x y z [w]
      else if(!splitline[0].compare("v")) {
        long double x = atof(splitline[1].c_str());
        long double y = atof(splitline[2].c_str());
        long double z = atof(splitline[3].c_str());
        ThreeDVector* vertex = new ThreeDVector(x, y, z);
        vertices.push_back(vertex);
      }
      //vn x y z
      else if(!splitline[0].compare("vn")) {
        long double x = atof(splitline[1].c_str());
        long double y = atof(splitline[2].c_str());
        long double z = atof(splitline[3].c_str());
        ThreeDVector* normal = new ThreeDVector(x, y, z);
        normal->normalize_bang();
        vertices_normals.push_back(normal);
      }
      //f v1 v2 v3 v4 ....
      //We assume they are all triangles defined with 3 vertices
      //Most files seem to be like this
      //1 indexed vertices so we need to add 1
      else if(!splitline[0].compare("f")) {
        vector<pair<ThreeDVector*, ThreeDVector*> > polygon;

        for(int i = 1; i < splitline.size(); i++){
          const char* v = splitline[i].c_str();
          char v_str[500];
          strncpy(v_str, v, sizeof(v_str));
          char *p = std::strtok(v_str, "/");
          vector<int> vector_of_indices; 
          while (p!=0){
            vector_of_indices.push_back(atoi(p)-1);
            p = strtok(NULL,"/");
          }
          ThreeDVector* vertex = vertices[vector_of_indices[0]];
          ThreeDVector* normal = vertices_normals[vector_of_indices[2]];

          pair<ThreeDVector*, ThreeDVector*> pair = std::make_pair(vertex, normal);
          polygon.push_back(pair);
        }

        polygons.push_back(polygon);
      }
    }
  }

  vertices.clear();
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
Viewport  viewport;

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
  //glEnable(GL_LIGHT1);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_RESCALE_NORMAL);

  //Set lighting parameters
  GLfloat light_position0[] = {0.0, 0.0, -1.0, 0};
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

  GLfloat light_position1[] = {0.0, 0.0, 1.0, 0};
  GLfloat light_ambient1[] = {0, 0, 0, 1};
  GLfloat light_diffuse1[] = {1.0, 1.0, 1.0, 1};
  GLfloat light_specular1[] = {1.0, 1.0, 1.0, 1};

  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);

  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);

  //Set Material Parameters
  GLfloat ambient_color[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat diffuse_color[] = { 0.0, 0.3, 0.3, 1.0 };
  GLfloat specular_color[] = { 0.3, 0.3, 0.3, 1.0 };
  GLfloat shininess[] = { 50.0 };
  GLfloat emission[] = {0, 0, 0, 1};

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);

  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  myReshape(viewport.w,viewport.h);
}

//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void myDisplay() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);       // clear the color buffer

  glMatrixMode(GL_MODELVIEW);         // indicate we are specifying camera transformations
  glLoadIdentity();                   // make sure transformation is "zero'd"

  if (wireframe) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  // Enable shading
  if (flat) {
    glShadeModel(GL_FLAT);
  } else {
    glShadeModel(GL_SMOOTH);
  }

  glScalef(scale_x, scale_y, scale_z); //Scale for Zooming in
  glRotatef(rotate_x, 1, 0, 0); // Rotation X
  glRotatef(rotate_y, 0, 1, 0); // Rotation Y
  glRotatef(rotate_z, 0, 0, 1); // Rotation Z
  glTranslatef(translate_x, translate_y, translate_z); //Translate for translations

  // Start drawing
  for(vector<vector<pair<ThreeDVector*, ThreeDVector*> > >::iterator it = polygons.begin(); it != polygons.end(); ++it) {
    vector<pair<ThreeDVector*, ThreeDVector*> > polygon = *it;
    glBegin(GL_POLYGON);                      // Draw A Polygon
    for(vector<pair<ThreeDVector*, ThreeDVector*> >::iterator i = polygon.begin(); i != polygon.end(); ++i) {
      pair<ThreeDVector*, ThreeDVector*> vertex_pair = *i;
      ThreeDVector* vertex = vertex_pair.first;
      ThreeDVector* normal = vertex_pair.second;
      glNormal3f(normal->x, normal->y, normal->z);
      glVertex3f(vertex->x, vertex->y, vertex->z);
    }
    glEnd();
  }

  if (save) {
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    vector<unsigned char> buf(w * h * 4);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &buf[0] );

    createPng(file_name, buf, w, h);
  }

  glFlush();
  glutSwapBuffers();          // swap buffers (we earlier set double buffer)
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
      } else if (line_count > 1) {
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

void uniform_subdivide(BezSurface* surface) {
  //compute how many subdivisions there are
  int EPSILION = 0.001;
  int num_subdivisions = (1 + EPSILION) / subdivision_parameter                                                                                                                                                                                                                                                       ;
  pair<ThreeDVector*, ThreeDVector*> surface_points[num_subdivisions + 1][num_subdivisions + 1];

  for (int iu = 0; iu < num_subdivisions + 1; iu++) {
    long double u = iu * subdivision_parameter;

    for (int iv = 0; iv < num_subdivisions + 1; iv++) {
      long double v = iv * subdivision_parameter;

      pair<ThreeDVector*, ThreeDVector*> surface_point = surface->interpolate(u, v);
      surface_points[iu][iv] = surface_point;
    }
  }

  //Interpolate the polygons from this surface_point mesh
  for (int u = 0; u < num_subdivisions; u++) {

    for (int v = 0; v < num_subdivisions; v++) {
      pair<ThreeDVector*, ThreeDVector*> UL = surface_points[u + 1][v];
      pair<ThreeDVector*, ThreeDVector*> UR = surface_points[u + 1][v + 1];
      pair<ThreeDVector*, ThreeDVector*> LR = surface_points[u][v + 1];
      pair<ThreeDVector*, ThreeDVector*> LL = surface_points[u][v];

      vector<pair<ThreeDVector*, ThreeDVector*> > polygon;
      polygon.push_back(UL);
      polygon.push_back(UR);
      polygon.push_back(LR);
      polygon.push_back(LL);
      polygons.push_back(polygon);
    }
  }

}

void generatePolygons() {
  if (adaptive) {

  } else {
    for (vector<BezSurface*>::iterator i = surfaces.begin(); i != surfaces.end(); ++i) {
      BezSurface* surface = *i;
      uniform_subdivide(surface);
    }
  }
}

void myKeyboardFunc(unsigned char key, int x, int y){
  switch(key){
    case KEY_S:
      flat = !flat;
      break;
    case KEY_W:
      wireframe = !wireframe;
      break;
    case KEY_PLUS:
      scale_x = max(scale_x + scale_step, scale_step);
      scale_y = max(scale_y + scale_step, scale_step);
      scale_z = max(scale_z + scale_step, scale_step);
      break;
    case KEY_MINUS:
      scale_x = max(scale_x - scale_step, scale_step);
      scale_y = max(scale_y - scale_step, scale_step);
      scale_z = max(scale_z - scale_step, scale_step);
      break;
    case KEY_H:
      print("Hidden Line");
      break;
    case KEY_P:
      {
      int w = glutGet(GLUT_WINDOW_WIDTH);
      int h = glutGet(GLUT_WINDOW_HEIGHT);
      vector<unsigned char> buf(w * h * 4);

      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &buf[0] );

      char* name = new char[1000];
      sprintf(name, "image%d.png", counter);
      createPng(name, buf, w, h);
      counter++;
      break;
      }
    default:
      break;
  }
  glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

void mySpecialKeyFunc(int key, int x, int y){
  bool shift = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
  switch(key){
    case GLUT_KEY_UP:
      if (shift) {
        translate_y += translate_step;
      } else {
        rotate_x += rotation_step;
      }
      break;
    case GLUT_KEY_DOWN:
      if (shift) {
          translate_y -= translate_step;
        } else {
        rotate_x -= rotation_step;
      }
      break;
    case GLUT_KEY_LEFT:
      if (shift) {
        translate_x -= translate_step;
      } else {
        rotate_y -= rotation_step;
      }
      break;
    case GLUT_KEY_RIGHT:
      if (shift) {
        translate_x += translate_step;
      } else {
        rotate_y += rotation_step;
      }
      break;
    default:
      break;
  }
  glutPostRedisplay(); // forces glut to call the display function (myDisplay())
}

int main(int argc, char *argv[]) {

  if (argc < 2) {
    cout << "USAGE: ./as3 [bez file] [subdivision parameter] [flags] OR ./as3 [obj file] [optional flags]" << endl;
    exit(1);
  }

  file_to_parse = argv[1];
  string fn = file_to_parse;
  string ext = fn.substr(fn.find_last_of(".") + 1);

  if (ext == "bez") {
    if (argc < 3) {
      cout << "USAGE: ./as3 [bez file] [subdivision parameter] [flags]" << endl;
      exit(1);
    }

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

    if (adaptive) {
      cout << "Adaptive" << endl;
    } else {
      cout << "Uniform" << endl;
    }

    parseBez(file_to_parse);
  } else if(ext == "obj"){
    //We are now dealing with .obj files

    for (int i = 2; i < argc; i++) {
      if (string(argv[i]) == "-save") {
        if(i + 1 < argc){
          save = true;
          file_name = argv[i + 1];
          i = i + 1;
        }
      }
    }

    parseObj(file_to_parse);
  } else {
    cout << "File Format Not Supported: " << argv[1] << endl;
    exit(1);
  }

  generatePolygons();

  //This initializes glut
  glutInit(&argc, argv);

  //This tells glut to use a double-buffered window with red, green, and blue channels. Add Depth Chanels
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  // Initalize theviewport size
  viewport.w = 400;
  viewport.h = 400;

  //The size and position of the window
  glutInitWindowSize(viewport.w, viewport.h);
  glutInitWindowPosition(0,0);
  glutCreateWindow(argv[0]);

  initScene();              // quick function to set up scene

  glutDisplayFunc(myDisplay);       // function to run when its time to draw something
  glutReshapeFunc(myReshape);       // function to run when the window gets resized
  glutKeyboardFunc(myKeyboardFunc); // basic keys callback
  glutSpecialFunc(mySpecialKeyFunc); //special keys callback

  glutMainLoop();             // infinite loop that will keep drawing and resizing
  // and whatever else

  return 0;
}








