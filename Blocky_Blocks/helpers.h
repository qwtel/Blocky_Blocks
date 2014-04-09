#pragma once

#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>
#include <GLFW\glfw3.h>

GLFWwindow* openWindow(int width, int height);

GLuint loadShader(GLenum shaderType, const char* shaderPath);

string readShader(const char * shaderPath);

GLuint compileShader(GLuint shaderID, string shaderCode);

GLuint createProgram(GLuint vertexShader, GLuint fragmentShader);