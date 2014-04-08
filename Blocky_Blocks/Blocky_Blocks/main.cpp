#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <GL\glew.h>
#include <GLFW\glfw3.h>

GLFWwindow* init();
void update(double deltaT);
void draw();
void Cleanup();

int WIDTH = 800;
int HEIGHT = 600;

int main() 
{
	// (1) init everything you need
	GLFWwindow* window = init();

	// Somewhere else
	glClearColor(0.14f, 0.25f, 0.43f, 0.0f);
	glViewport(0, 0, WIDTH, HEIGHT);

	bool running = true;
	double lastTime = 0;
	double deltaT;

	while (running && !glfwWindowShouldClose(window))
	{
		// (2) clear the frame and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// (3) compute the frame time delta
		double time = glfwGetTime();
		deltaT = time - lastTime;
		lastTime = time;

		// (4) react to user input
		glfwPollEvents();
		running = !glfwGetKey(window, GLFW_KEY_ESCAPE);

		// (5) Update all game components
		update(deltaT);

		// (6) draw all game components
		draw();
		glfwSwapBuffers(window);

		// (7) check for errors
		if (glGetError() != GL_NO_ERROR) {
			std::cout << "GL ERROR DETECTED!!!" << std::endl;
		}
	}

	// (8) clean up!
	Cleanup();
	glfwTerminate();
}

GLFWwindow* init() 
{

	glewExperimental = GL_TRUE; 

	// (1) init glfw
	if (!glfwInit()) 
	{
		std::cout << "Failed to init glfw" << std::endl;
		system("PAUSE");
		exit(-1);
	}

	// (2) set window hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// (3) open window
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Hello, CGUE!", nullptr, nullptr);
	if (!window)
	{
		glfwTerminate();
		std::cout << "Failed to open window" << std::endl;
		system("PAUSE");
		exit(-1);
	}

	glfwMakeContextCurrent(window);

	// (4) init glew
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize glew" << std::endl;
		system("PAUSE");
		exit(-1);
	}

	std::cout << "Alles super!!11" << std::endl;

	double deltaT;
	double time = glfwGetTime();
	double lastTime = time;

    return window;
}
void update(double deltaT) 
{
}
void draw() 
{}
void Cleanup() {}