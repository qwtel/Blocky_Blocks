#include "Shader.h"

#include <vector>
#include <fstream>

Shader::Shader(const char* filePath, GLenum shaderType)
    : Holder()
{
    fprintf(stdout, "Reading shader : %s\n", filePath);
    string shaderCode = Shader::readFile(filePath);

    _object = glCreateShader(shaderType);

    fprintf(stdout, "Compiling shader\n");
    compileShader(shaderCode);
}

GLuint Shader::compileShader(string shaderCode)
{
    GLint result = GL_FALSE;
    int infoLogLength;

    char const * shaderSourcePointer = shaderCode.c_str();
    glShaderSource(_object, 1, &shaderSourcePointer , nullptr);
    glCompileShader(_object);

    // Check Shader
    glGetShaderiv(_object, GL_COMPILE_STATUS, &result);
    glGetShaderiv(_object, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 )
    {
        vector<char> vertexShaderErrorMessage(infoLogLength+1);
        glGetShaderInfoLog(_object, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        fprintf(stderr, "%s\n", &vertexShaderErrorMessage[0]);
    }

    return _object;
}


Shader::~Shader(void) 
{
    if (_object != 0) {
	    glDeleteShader(_object);
    }
}

string Shader::readFile(const char* filePath)
{
    string shaderCode;
    ifstream shaderStream(filePath, ios::in);
    if (shaderStream.is_open())
    {
        string line = "";
        while(getline(shaderStream, line))
        {
            shaderCode += "\n" + line;
        }
        shaderStream.close();
    } 
    else
    {
        fprintf(stderr, "Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", filePath);
        system("PAUSE");
        exit(-1);
    }

    return shaderCode;
}