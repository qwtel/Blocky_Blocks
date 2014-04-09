#include <stdio.h>
#include <stdlib.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

/*
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;
*/

GLFWwindow* openWindow();
void update(double deltaT);
void draw();
void Cleanup();

int WIDTH = 800;
int HEIGHT = 600;

#include "Tutorial/Person.h"
#include "Tutorial/Student.h"

int main() 
{

    Person* p = new Person("Florian");
    fprintf(stdout, "%s\n", p->getName().c_str());
    fprintf(stdout, "%s\n", p->saySomething().c_str());

    Student* s = new Student("Florian", 1058208);

    fprintf(stdout, "%s\n", s->saySomething().c_str());

    delete p;
    delete s;

    // (1) init everything you need
    GLFWwindow* window = openWindow();

    // Somewhere else
    glClearColor(0.14f, 0.25f, 0.43f, 0.0f);
    glViewport(0, 0, WIDTH, HEIGHT);

    bool running = true;
    double lastTime = 0;
    double deltaT;

    while (running && !glfwWindowShouldClose(window))
    {
        // (2) clear the frame and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // (3) compute the frame time delta
        double time = glfwGetTime();
        deltaT = time - lastTime;
        lastTime = time;

        // (4) react to user input
        glfwPollEvents();
        running = !glfwGetKey(window, GLFW_KEY_ESCAPE);

        // (5) Update all game components
        update(deltaT);

        // (6) draw all game components
        draw();
        glfwSwapBuffers(window);

        // (7) check for errors
        if (glGetError() != GL_NO_ERROR) {
            fprintf(stderr, "GL ERROR DETECTED!!!"); 
        }
    }

    // (8) clean up!
    Cleanup();
    glfwTerminate();
}

GLFWwindow* openWindow() 
{
    // (1) init glfw
    if (!glfwInit()) 
    {
        fprintf(stderr, "Failed to init glfw");
        system("PAUSE");
        exit(-1);
    }

    // (2) set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // (3) open window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello, CGUE!", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "Failed to open window"); 
        system("PAUSE");
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE; 

    // (4) init glew
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize glew");
        system("PAUSE");
        exit(-1);
    }

    fprintf(stdout, "Opened OpenGL Window");

    return window;
}

void update(double deltaT) 
{
}

void draw() 
{
}

void Cleanup()
{
}