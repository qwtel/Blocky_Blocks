#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "helpers.h"
#include "controls.h"

#include "Holder/Shader.h"
#include "Holder/Program.h"

#include "Scene/Triangle.h"
#include "Scene/Block.h"

int WIDTH = 800;
int HEIGHT = 600;

GLFWwindow* window;

Triangle* t;
Block* b;

void update(double deltaT);
void draw();
void cleanup();

int main() 
{
    // (1) init everything you need
    window = openWindow(WIDTH, HEIGHT);

    Shader vertexShader = Shader("Blocky_Blocks/Shader/textureVertexshader.vert", GL_VERTEX_SHADER);
    Shader fragmentShader = Shader("Blocky_Blocks/Shader/textureFragmentshader.frag", GL_FRAGMENT_SHADER);
    Program program = Program(vertexShader, fragmentShader);

    // Get a handle for our "MVP" uniform
    GLuint matrixID = glGetUniformLocation(program.object(), "mvp");

    t = new Triangle(program);
    // b = new Block(matrixID);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    bool running = true;
    double lastTime = 0;

    while (running && !glfwWindowShouldClose(window))
    {
        // (2) clear the frame and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        // (6.5) swap buffers
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
    computeMatricesFromInputs(window, float(deltaT));

    // b->update(deltaT);
}

void draw() 
{
    // TODO: loop through scene graph?

    mat4 projectionMatrix = getProjectionMatrix();
    mat4 viewMatrix = getViewMatrix();

    t->draw();

    // b->draw(projectionMatrix * viewMatrix);
}

void cleanup()
{
    delete t;
    // delete b;

    // glDeleteVertexArrays(1, &vao);
}