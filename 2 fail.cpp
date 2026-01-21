#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <chrono>
#include <fstream>
#include <sstream>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
using namespace glm;
using namespace std;
using Clock = std::chrono::high_resolution_clock;

double lastPrintTime = 0.0;
int    framesCount   = 0;
double c = 299792458.0;
double G = 6.67430e-11;
struct Ray;
bool Gravity = false;

struct Camera{

    vec3 target = vec3(0.0f, 0.0f, 0.0f);
    float radius = 6.34194e10f;
    float minRadius = 1e10f, maxRadius = 1e12f;

    float azimuth = 0.0f;
    float elavetion = M_PI / 2.0f;

    float orbitSpeed = 0.01f;
    float panSpeed = 0.01f;
    double zoomSpeed = 25e9f;

    bool dragging = false;
    bool panning = false;
    bool moving = false;
    double lastX = 0.0, lastY = 0.0;

    vec3 position() const {
        float clambedElevation = glm::clamp(elavetion, 0.01f, float(M_PI) - 0.01f);
        return vec3(
            radius * sin(clambedElevation) * cos(azimuth),
            radius * cos(clambedElevation),
            radius * sin(clambedElevation) * sin(azimuth)
        );
    }
    void update(){

        target = vec3(0.0f, 0.0f, 0.0f);
        if(dragging | panning){
            moving = true;
        } else{
            moving = false;
        }
    }
    void processMouseMove(double x, double y) {
        float dx = float(x - lastX);
        float dy = float(y - lastY);

        if (dragging && panning) {

        }
        else if (dragging && !panning){

            azimuth  += dx * orbitSpeed;
            elavetion -= dy * orbitSpeed;
            elavetion = glm::clamp(elavetion, 0.01f, float(M_PI) - 0.01f);
        }
        lastX = x;
        lastY = y;
        update();
    }
    void proccesMouseButton(int button, int action, int mods, GLFWwindow* win){
        if (button == GLFW_MOUSE_BUTTON_LEFT || button == GLFW_MOUSE_BUTTON_MIDDLE){
            if (action == GLFW_PRESS){
                dragging = true;
                panning = false;
                glfwGetCursorPos(win, &lastX, &lastY);
            }
            else if (action == GLFW_RELEASE){
                dragging = false;
                panning = false;
            }
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT){
            if (action == GLFW_PRESS){
                Gravity = true;
            }
            else if (action == GLFW_RELEASE){
                Gravity = false;
            }
        }
    }
    void processScroll(double xoffset, double yoffset){
        radius -= yoffset * zoomSpeed;
        radius = glm::clamp(radius, minRadius, maxRadius);
        update();
    }
    void processKey(int key, int scandone, int action,int mods){
        if (action == GLFW_PRESS && key == GLFW_KEY_G){
            Gravity = !Gravity;
            cout << "[INFO] Gravity turned " << (Gravity ? "ON" : "OFF") <<endl;
        }
    }
};
Camera camera;

struct BlackHole {
    vec3 position;
    double mass;
    double radius;
    double r_s;

    BlackHole(vec3 pos, float m) : position(pos), mass(m) {r_s = 2.0 * G * mass / (c*c);}
    bool Intercept(float px, float py, float pz) const {
        double dx = double(px) - double(position.x);
        double dy = double(py) - double(position.y);
        double dz = double(pz) - double(position.z);
        double dist2 = dx * dx + dy * dy + dz * dz;
        return dist2 < r_s * r_s;
    }
};
BlackHole SagA(vec3(0.0f, 0.0f, 0.0f), 8.54e36);
struct ObjectData {
    vec4 posRadius;
    vec4 color;
    float mass;
    vec3 velocity = vec3(0.0f, 0.0f, 0.0f);
};
vector<ObjectData> objects = {
    { vec4(4e11f, 0.0f, 0.0f, 4e10f)   , vec4(1,1,0,1), 1.98892e30},
    { vec4(0.0f, 0.0f, 4e11f, 4e10f)   , vec4(1,0,0,1), 1.98892e30},
    { vec4{0.0f, 0.0f, 0.0f, SagA.r_s} , vec4(0,0,0,1), static_cast<float>(SagA.mass) },
};
struct Engine {
    GLuint gridShaderProgram;
    GLFWwindow* window;
    GLuint quadVAO;
    GLuint texture;
    GLuint shaderProgram;
    GLuint computerProgram = 0;

    GLuint cameraUBO = 0;
    GLuint diskUBO = 0;
    GLuint objectUBO = 0;

    GLuint gridVAO = 0;
    GLuint gridVBO = 0;
    GLuint gridEBO = 0;
    int gridIndexCount = 0;

    int WIDTH = 800;
    int HEIGHT = 600;
    int COMPUTE_WIDTH = 200;
    int COMPUTE_HEIGHT = 150;
    float width = 100000000000.0f;
    float height = 75000000000.0f;

    Engine() {
        if (!glfwInit()){
            cerr << "GLFW init failed\n";
            exit(EXIT_FAILURE);
        }
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "Black hole artem", nullptr, nullptr);
        if(!window) {
            cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        glfwMakeContextCurrent(window);
        glewExperimental = GL_TRUE;
        GLenum glewErr = glewInit();
        if (glewErr != GLEW_OK) {
            cerr << "Failed to initialize GLEW: " << (const char*)glewGetErrorString(glewErr) << "\n";
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
        this->shaderProgram = CreateShaderProgram();
        gridShaderProgram = CreateShaderProgram();
    }
    
};
