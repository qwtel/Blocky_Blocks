#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

#include "Player.h"

class Camera
{
public:
    Camera(Player* player);
    ~Camera(void);

    mat4 matrix() const;
    mat4 orientation() const;

    const vec3 position() const;

    void offsetOrienatation(float upAngle, float rightAngle);

    void setViewportAspectRatio(float ratio);

    vec3 up() const;
    vec3 forward() const;
    vec3 right() const;

private:
    Player* _target;
    vec3 _distance;
    float _fieldOfView;
    float _viewportAspectRatio;
    float _nearPlane;
    float _farPlane;

    void normalizeAngles();
};

