#include <vec3.hpp>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <gtc/matrix_transform.hpp>
using namespace glm;

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "quad.h"

GLFWwindow* window;
char* vss =
"#version 330 core \n"
"layout (location = 0) in vec3 position;"
"layout (location = 1) in vec3 vertexcolor;"
"out vec3 color;"
"uniform mat4 projection;"
"uniform mat4 view;"
"uniform mat4 model;"
" "
"void main() {"
"    color = vertexcolor;"
"    gl_Position = projection * view * model * vec4(position, 1.0);"
"}";

char* fss =
"#version 330 core \n"
"out vec4 FragColor;"
"in vec3 color;"
" "
"void main() {"
"    FragColor = vec4(color, 1.0);"
"}";

vec3 camera_position = vec3(0,0,0);
vec3 camera_eye = vec3(0,0,0);
vec3 camera_up = vec3(0,1,0);

mat4 model;
mat4 view;
mat4 projection;

vec2 lastMousePosition = vec2(0,0);
vec3 mouseRay;
vec3 last_camera_position;

Renderable renderables[3];


float xrot, yrot, CAMERA_SPEED = .5f;
bool isMouseDown = false;
float iterations = 0;


void keyboard(GLFWwindow* window, Renderable* quad_array);
void cursorPosition(GLFWwindow* window, double xpos, double ypos);
void mouseButton(GLFWwindow* window, int b, int a, int m);
float clamp(float v, float min, float max);


int main(int argc, const char *argv[]) {
    if (!glfwInit()) glfwTerminate();
            
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    GLFWwindow* window = glfwCreateWindow(1200, 800, "2030", NULL, NULL);
    
    if (!window) glfwTerminate();
    
    glfwSetCursorPosCallback(window, cursorPosition);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetMouseButtonCallback(window, mouseButton);
    
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    
    Renderable r[3] = {
        Quad(vss, fss, vec3(0.5,0.5,-10.0), vec3(1,0,0), vec3(0,1,0), 1.0, 0.0, 0.0),
        Quad(vss, fss, vec3(1,0,0), vec3(0,1,0), vec3(0.5,0.5,1.0), 0.0, 1.0, 0.0),
        Quad(vss, fss, vec3(1,0,0), vec3(0,1,0), vec3(0.5,0.5,2.0), 0.0, 0.0, 1.0),
    };
    
    for (int i = 0; i < 3; i++) {
        renderables[i] = r[i];
    }
    
    glEnable(GL_DEPTH_TEST);
                
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        projection = perspective(radians(90.f), 1.f, .01f, 2000.f);
        view = lookAt(camera_position, camera_position+camera_eye, camera_up);
        model = mat4(1.0f);
        
        for (int i = 0; i < 2; i++) {
            renderables[i].render(model, view, projection);
        }
        
        keyboard(window, renderables);
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    glfwTerminate();
}

void cursorPosition(GLFWwindow* window, double xpos, double ypos) {
    if (isMouseDown) {
        
        double xdist = lastMousePosition.x - xpos;
        double ydist = lastMousePosition.y - ypos;
        
        xrot += ydist * .5f;
        yrot -= xdist * .5f;
        
        xrot = clamp(xrot, -89.9f, 89.9f);
        
        camera_eye.x = cos(radians(xrot)) * cos(radians(yrot));
        camera_eye.y = sin(radians(xrot));
        camera_eye.z = cos(radians(xrot)) * sin(radians(yrot));
        
        camera_eye = normalize(camera_eye);
    }
    lastMousePosition = vec2(xpos, ypos);
}

void mouseButton(GLFWwindow* window, int b, int a, int m) {
    if (b == GLFW_MOUSE_BUTTON_RIGHT && a == GLFW_PRESS) {
        isMouseDown = true;
    }
    
    if (b == GLFW_MOUSE_BUTTON_LEFT && a == GLFW_PRESS) {
        vec2 mouseCoords = vec2(2*lastMousePosition.x/1200-1, 2*(800-lastMousePosition.y)/800-1);
        vec4 clipCoords = vec4(mouseCoords.x, mouseCoords.y, -1.f, 1.f);
        
        mat4 invertedProjection = inverse(projection);
        vec4 eyeSpace = vec4((invertedProjection*clipCoords).x, (invertedProjection*clipCoords).y, -1.f, 0.f);
        
        mat4 invertedView = inverse(view);
        vec4 ray = invertedView * eyeSpace;
        vec3 mouseray = vec3(ray.x, ray.y, ray.z);
        mouseray = normalize(mouseray);
        mouseRay = mouseray;
        iterations = 0;
        last_camera_position = camera_position;
    }
    
    if (b == GLFW_MOUSE_BUTTON_RIGHT && a == GLFW_RELEASE) {
        isMouseDown = false;
    }
}


// CAMERA

float clamp(float v, float min, float max) {
    if (v > max) return max;
    if (v < min) return min;
    
    return v;
}

void keyboard(GLFWwindow* window, Renderable quad_array[]) {
    
    // vertical
         if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) camera_position += camera_eye * CAMERA_SPEED;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) camera_position -= camera_eye * CAMERA_SPEED;
    
    // horizontal
    vec3 right = normalize(cross(camera_eye, camera_up));
         if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) camera_position -= right * CAMERA_SPEED;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) camera_position += right * CAMERA_SPEED;
    
    projection = perspective(radians(90.f), 1.f, .01f, 2000.f);
    view = lookAt(camera_position, camera_position+camera_eye, camera_up);
    model = mat4(1.0f);
    
    for (int i = 0; i < 2; i++) {
        quad_array[i].render(model, view, projection);
    }
    
    
    vec3 rot = mouseRay;
    
    quad_array[0].setPosition(vec3(1,1,1), rot);
    
    
    quad_array[2].setPosition(last_camera_position+mouseRay*iterations, rot);
    quad_array[2].render(model, view, projection);
    iterations++;
}
