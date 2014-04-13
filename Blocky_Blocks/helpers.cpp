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

    glGetError();

    fprintf(stdout, "Opened OpenGL Window.\n");

    // glClearColor(0.14f, 0.25f, 0.43f, 0.0f);
    glViewport(0, 0, width, height);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPos(window, double(width) / 2.0, double(height) / 2.0);

    return window;
}