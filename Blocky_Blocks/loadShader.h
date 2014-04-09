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

GLuint loadShader(GLenum shaderType, const char* shaderPath);
string readShader(const char * shaderPath);
GLuint compileShader(GLuint shaderID, string shaderCode);