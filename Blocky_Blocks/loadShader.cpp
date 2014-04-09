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

// #include "shader.hpp"

GLuint loadShader(const char * shaderPath, GLuint shaderType) {
    // Create the shaders
    GLuint shaderID = glCreateShader(shaderType);

    // Read the Vertex Shader code from the file
    string vertexShaderCode;
    ifstream vertexShaderStream(shaderPath, std::ios::in);
    if (vertexShaderStream.is_open())
    {
        string line = "";
        while(getline(vertexShaderStream, line))
        {
            vertexShaderCode += "\n" + line;
        }
        vertexShaderStream.close();
    } 
    else
    {
        fprintf(stderr, "Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", shaderPath);
	system("PAUSE");
        return 0;
    }
}