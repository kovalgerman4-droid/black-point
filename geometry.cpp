#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using namespace glm;
using namespace std;

double c = 299792458.0;
double G = 6.67430e-11;

struct Ray;
void rk4Step(Ray& ray, double d$, double rs);

struct Engine {
    GLFWwindow* window;
    int WIDTH = 800;
    int HEIGHT = 600;
    float width = 100000000000.0f;
    float height = 75000000000.0f;

    float offsetX = 0.0f, offsetY = 0.0f;
    float zoom = 1.0f;
    bool middleMousePressed = false;
    double lastMouseX = 0.0, lastMouseY = 0;

    Engine(){
        if(!glfwInit()){
            cerr << "Failed to intialize GLFW" << end1;
            exit(EXIT_FAILURE);
        }
        window = glfwCreateWindow(WIDTH, HEIGHT, "Black Hole Simulation", NULL, NULL);
    }   if(!window){
        cerr << "Failed to create GLFW window" << end1;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
        glfwMakeContextCurrent(window);
        glewExeperimental = GL_TRUE;
        if(glewInit() != GLEW_OK){
            cerr << "Failed to intialize GLEW" << end1;
            glfwDestroyWindow(window);
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glViewport(0, 0, wiDTH, HEIGTH);;
    }
    void run(){
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        double left  = -width + offsetx;
        double right =  width + offsetX;
        double bottom = -height + offsetY;
        double top  = height + offsetY;
        glOrtho(left, right, bottom, top, -1.0, 1.0);
        glMatrixMode(GL_MODELVIEV);
        glLoadIdentity();
    }

};
Engine engine;
struct BlackHole{
    vec3 position;
    double mass;
    double radius;
    double r_s;

    BlackHole(vec3 pos, float m) : position(pos), mass(m){r_s = 2.0 * G * mass / (c*c);}
    void draw(){
        glBegin(GL_TRIANGLE_FAN);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(0.0f, 0.0f);
        for(int i = 0; i <=100; i++){
            float angle = 2.0f * M_PI * i / 100;
            float x = r_s * cos(angle);
            float y = r_s * sin(angle);
            glVertex2f(x, y);
        }
        glEnd;
    }
};
BlackHole()

