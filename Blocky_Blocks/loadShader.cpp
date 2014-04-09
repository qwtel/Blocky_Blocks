#include "loadShader.h"

GLuint loadShader(GLenum shaderType, const char* shaderPath)
{
    // Create the Shader
    GLuint shaderID = glCreateShader(shaderType);

    // Read the Shader code from the file
    fprintf(stdout, "Reading shader : %s\n", shaderPath);
    string shaderCode = readShader(shaderPath);

    // Compile Shader
    fprintf(stdout, "Compiling shader : %s\n", shaderPath);
    return compileShader(shaderID, shaderCode);

}

string readShader(const char* shaderPath)
{
    string shaderCode;
    ifstream vertexShaderStream(shaderPath, std::ios::in);
    if (vertexShaderStream.is_open())
    {
        string line = "";
        while(getline(vertexShaderStream, line))
        {
            shaderCode += "\n" + line;
        }
        vertexShaderStream.close();
    } 
    else
    {
        fprintf(stderr, "Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", shaderPath);
        system("PAUSE");
	exit(-1);
    }

    return shaderCode;
}

GLuint compileShader(GLuint shaderID, string shaderCode)
{
    GLint result = GL_FALSE;
    int infoLogLength;

    char const * shaderSourcePointer = shaderCode.c_str();
    glShaderSource(shaderID, 1, &shaderSourcePointer , nullptr);
    glCompileShader(shaderID);

    // Check Shader
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if ( infoLogLength > 0 )
    {
        vector<char> vertexShaderErrorMessage(infoLogLength+1);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
        fprintf(stderr, "%s\n", &vertexShaderErrorMessage[0]);
    }

    return shaderID;
}