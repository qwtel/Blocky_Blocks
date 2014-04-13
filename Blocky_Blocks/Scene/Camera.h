#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class Camera
{
public:
    Camera(void);
    ~Camera(void);

    mat4 matrix() const;
    mat4 orientation() const;

    const vec3& position() const;
    void setPosition(const vec3& position);
    void offsetPosition(const vec3& offset);

    void offsetOrienatation(float upAngle, float rightAngle);

    void setViewportAspectRatio(float ratio);

    vec3 up() const;
    vec3 forward() const;
    vec3 right() const;

private:
    vec3 _position;
    float _horizontalAngle;
    float _verticalAngle;
    float _fieldOfView;
    float _viewportAspectRatio;
    float _nearPlane;
    float _farPlane;

    void normalizeAngles();
    };

