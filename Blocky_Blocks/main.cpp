#include <stdio.h>
#include <stdlib.h>

#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "helpers.h"
#include "Scene\Triangle.h"
#include "Scene\Block.h"

int WIDTH = 800;
int HEIGHT = 600;

GLuint vertexShader;
GLuint fragmentShader;
GLuint program;
GLuint vao;

Triangle* t;
Block* b;

void update(double deltaT);
void draw();
void cleanup();

int main() 
{
    // (1) init everything you need
    GLFWwindow* window = openWindow(WIDTH, HEIGHT);

    bool running = true;
    double lastTime = 0;

    vertexShader = loadShader(GL_VERTEX_SHADER, "Shader/vertexshader.vert");
    fragmentShader = loadShader(GL_FRAGMENT_SHADER, "Shader/fragmentshader.frag");
    program = createProgram(vertexShader, fragmentShader);

    // This is evil..
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // <MPV>
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(program, "mvp");

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    mat4 projection = perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

    // Or, for an ortho camera :
    //mat4 Projection = ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

    // Camera matrix
    mat4 view = lookAt(
        vec3(4,3,3), // Camera is at (4,3,3), in World Space
        vec3(0,0,0), // and looks at the origin
        vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

    // Model matrix : an identity matrix (model will be at the origin)
    mat4 model = mat4(1.0f);

    // Our ModelViewProjection : multiplication of our 3 matrices
    mat4 mvp = projection * view * model; // Remember, matrix multiplication is the other way around
    // </MPV>

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    t = new Triangle();
    b = new Block();

    while (running && !glfwWindowShouldClose(window))
    {
        // (2) clear the frame and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        // <MPV>
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        // </MPV>

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
    cleanup();

    glfwTerminate();
}

void update(double deltaT) 
{
}

void draw() 
{
    // loop through scene graph?
    // t->draw();
    b->draw();
}

void cleanup()
{
    delete t;
    delete b;

    glDeleteProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteVertexArrays(1, &vao);
}