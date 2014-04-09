#pragma once

#include <GLFW\glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

void computeMatricesFromInputs(GLFWwindow* window, float deltaT);

mat4 getProjectionMatrix();

mat4 getViewMatrix();