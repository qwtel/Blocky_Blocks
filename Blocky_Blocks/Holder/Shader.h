#pragma once

#include <GL\glew.h>
#include <string>

#include "Holder.h"

using namespace std;

class Shader : public Holder
{
public:
    Shader(const char* filePath, GLenum shaderType);
    virtual ~Shader(void);

private:
    GLuint compileShader(string shaderCode);
    static string readFile(const char* filePath);
};

