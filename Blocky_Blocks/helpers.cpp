#include "helpers.h"

GLFWwindow* openWindow(int width, int height) 
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
    GLFWwindow* window = glfwCreateWindow(width, height, "Hello, CGUE!", nullptr, nullptr);
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

    // Somewhere else
    // glClearColor(0.14f, 0.25f, 0.43f, 0.0f);
    glViewport(0, 0, width, height);

    return window;
}

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
