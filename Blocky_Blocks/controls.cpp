#include "controls.h"

mat4 viewMatrix = mat4();
mat4 projectionMatrix = mat4();

// Initial position : on +Z
vec3 position = vec3(0, 0, 5); 

// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;

// Initial vertical angle : none
float verticalAngle = 0.0f;

// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.01f;

void computeMatricesFromInputs(GLFWwindow* window, float deltaT)
{
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);

    int xMax, yMax;
    glfwGetWindowSize(window, &xMax, &yMax);

    float xCenter = xMax/2;
    float yCenter = yMax/2;

    // Reset mouse position for next frame
    glfwSetCursorPos(window, xCenter, yCenter);

    // We can now compute our viewing angles 
    float xFromCenter = float(xCenter - xPos);
    float yFromCenter = float(yCenter - yPos);

    horizontalAngle += mouseSpeed * deltaT * xFromCenter;
    verticalAngle   += mouseSpeed * deltaT * yFromCenter;

    // Direction : Spherical coordinates to Cartesian coordinates conversion
    vec3 direction = vec3(
        cos(verticalAngle) * sin(horizontalAngle),
        sin(verticalAngle),
        cos(verticalAngle) * cos(horizontalAngle)
        );

    // Right vector
    vec3 right = vec3(
        sin(horizontalAngle - 3.14f/2.0f),
        0,
        cos(horizontalAngle - 3.14f/2.0f)
        );

    vec3 up = cross(right, direction);

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        position += direction * deltaT * speed;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        position -= direction * deltaT * speed;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        position += right * deltaT * speed;
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_LEFT ) == GLFW_PRESS){
        position -= right * deltaT * speed;
    }

    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    projectionMatrix = perspective(initialFoV, 4.0f / 3.0f, 0.1f, 100.0f);

    // Camera matrix
    viewMatrix = lookAt(
        position,             // Camera is here
        position + direction, // and looks here : at the same position, plus "direction"
        up                    // Head is up (set to 0,-1,0 to look upside-down)
        );

}

mat4 getViewMatrix()
{
    return viewMatrix;
}

mat4 getProjectionMatrix()
{
    return projectionMatrix;
}
