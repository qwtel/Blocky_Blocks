#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "helpers.h"
#include "controls.h"

#include "Holder/Shader.h"
#include "Holder/Program.h"

#include "Scene/Camera.h"
#include "Scene/Triangle.h"
#include "Scene/Block.h"

const vec2 SCREEN_SIZE(800, 600);
const vec2 CENTER = SCREEN_SIZE * 0.5f;

GLFWwindow* window;

Program* program;
Camera camera;

Triangle* t;
Block* b;

void update(double deltaT);
void draw();
void cleanup();

int main() 
{
    // (1) init everything you need
    window = openWindow(SCREEN_SIZE.x, SCREEN_SIZE.y);
    Shader vertexShader = Shader("Blocky_Blocks/Shader/cameraVertexShader.vert", GL_VERTEX_SHADER);
    Shader fragmentShader = Shader("Blocky_Blocks/Shader/textureFragmentshader.frag", GL_FRAGMENT_SHADER);
    program = new Program(vertexShader, fragmentShader);

    // bind the program (the shaders)
    glUseProgram(program->object());

    camera = Camera();
    camera.setPosition(vec3(0,0,4));
    camera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    //t = new Triangle(*program);
    b = new Block(*program);

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
        GLenum errCode;
        if ((errCode = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr, "OpenGL Error: %i\n", errCode);
        }
    }

    // (8) clean up!
    cleanup();

    glfwTerminate();
}

void update(double deltaT) 
{
    //move position of camera based on WASD keys
    const float moveSpeed = 2.0; //units per second

    float deltaTf = float(deltaT);

    if(glfwGetKey(window, 'S')){
    camera.offsetPosition(deltaTf * moveSpeed * -camera.forward());
    } else if(glfwGetKey(window, 'W')){
    camera.offsetPosition(deltaTf * moveSpeed * camera.forward());
    }

    if(glfwGetKey(window, 'A')){
    camera.offsetPosition(deltaTf * moveSpeed * -camera.right());
    } else if(glfwGetKey(window, 'D')){
    camera.offsetPosition(deltaTf * moveSpeed * camera.right());
    }

    if(glfwGetKey(window, 'Z')){
    camera.offsetPosition(deltaTf * moveSpeed * -vec3(0,1,0));
    } else if(glfwGetKey(window, 'X')){
    camera.offsetPosition(deltaTf * moveSpeed * vec3(0,1,0));
    }

    //rotate camera based on mouse movement
    const float mouseSensitivity = 0.1;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float diffX = mouseX - CENTER.x;
    float diffY = mouseY - CENTER.y;

    camera.offsetOrienatation(mouseSensitivity * diffY, mouseSensitivity * diffX);

    glfwSetCursorPos(window, CENTER.x, CENTER.y); //reset the mouse, so it doesn't go out of the window

    b->update(deltaT);
}

void draw() 
{
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, value_ptr(camera.matrix()));

    // TODO: loop through scene graph?

    //mat4 projectionMatrix = getProjectionMatrix();
    //mat4 viewMatrix = getViewMatrix();

    // t->draw();


    b->draw();
}

void cleanup()
{
    glUseProgram(0);

    delete program;
    // delete t;
    delete b;

    // glDeleteVertexArrays(1, &vao);
}