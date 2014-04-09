#include <stdio.h>
#include <stdlib.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
using namespace glm;

GLFWwindow* openWindow();
void update(double deltaT);
void draw();
void cleanup(GLuint program, GLuint vertexShader, GLuint fragmentShader);

int WIDTH = 800;
int HEIGHT = 600;

#include "loadShader.h"
#include "Scene\Triangle.h"

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);

Triangle* t;

int main() 
{
    // (1) init everything you need
    GLFWwindow* window = openWindow();

    // Somewhere else
    glClearColor(0.14f, 0.25f, 0.43f, 0.0f);
    glViewport(0, 0, WIDTH, HEIGHT);

    bool running = true;
    double lastTime = 0;

    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, "Shader/vertexshader.vert");
    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, "Shader/fragmentshader.frag");
    GLuint program = createProgram(vertexShader, fragmentShader);

    // This is evil..
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    t = new Triangle();

    while (running && !glfwWindowShouldClose(window))
    {
        // (2) clear the frame and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        // (3) compute the frame time delta
        double time = glfwGetTime();
        double deltaT = time - lastTime;
        lastTime = time;

        // (4) react to user input
        glfwPollEvents();
        running = !glfwGetKey(window, GLFW_KEY_ESCAPE);

        // (5) Update all game components
        update(deltaT);

        // (6) draw all game components
        draw();

        // (6.5) bla
        glfwSwapBuffers(window);

        // (7) check for errors
        if (glGetError() != GL_NO_ERROR) 
        {
            fprintf(stderr, "GL ERROR DETECTED!!!\n"); 
        }
    }

    // (8) clean up!
    cleanup(program, vertexShader, fragmentShader);

    glDeleteVertexArrays(1, &VertexArrayID);

    glDeleteProgram(program);

    glfwTerminate();
}

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader)
{
    // Create program
    GLuint program = glCreateProgram();

    if (program == 0)
    {
        fprintf(stderr, "Failed to create shader program");
        system("PAUSE");
        exit(-1);
    }

    // Attach shader to program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Bind output
    glBindFragDataLocation(program, 0, "fragColor");

    // Link programs
    glLinkProgram(program);

    // check for errors
    GLint succeded;
    glGetProgramiv(program, GL_LINK_STATUS, &succeded);

    if (!succeded)
    {
        // output errors
        GLint logSize;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);

        auto message = new char[logSize];
        glGetProgramInfoLog(program, logSize, NULL, message);

        fprintf(stderr, "Failed to link shader program");
        fprintf(stderr, "%s", message);
        system("PAUSE");

        delete[] message;
        exit(-1);
    }

    return program;
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

    fprintf(stdout, "Opened OpenGL Window.\n");

    return window;
}

void update(double deltaT) 
{
}

void draw() 
{
    // loop through scene graph?
    t->draw();
}

void cleanup(GLuint program, GLuint vertexShader, GLuint fragmentShader)
{
    delete t;

    glDeleteProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}