#include "Camera.h"

vec3 Distance = vec3(0, 4, -10);

Camera::Camera(Player* player) :
    _target(player),
    _distance(Distance),
    _fieldOfView(50.0f), 
    _viewportAspectRatio(4.0f/3.0f), 
    _nearPlane(0.1f), 
    _farPlane(300.0f)
{
}

Camera::~Camera(void)
{
}

mat4 Camera::matrix() const {
    mat4 projection = glm::perspective(_fieldOfView, _viewportAspectRatio, _nearPlane, _farPlane);
    vec3 center = _target->position() + vec3(0,2,0); // look a bit above the target
    mat4 view = glm::lookAt(position(), center, vec3(0,1,0));
    return projection * view;
}

mat4 Camera::projectionMatrix() const {
    mat4 projection = perspective(_fieldOfView, _viewportAspectRatio, _nearPlane, _farPlane);
    return projection;
}

mat4 Camera::viewMatrix() const {
    vec3 center = _target->position() + vec3(0,2,0); // look a bit above the target
    mat4 view = glm::lookAt(position(), center, vec3(0,1,0));
    return view;
}

// @Depriacted
mat4 Camera::orientation() const {
    //mat4 orientation;
    //orientation = rotate(orientation, _verticalAngle, vec3(1,0,0));
    //orientation = rotate(orientation, _horizontalAngle, vec3(0,1,0));
    //return orientation;
    return mat4();
}

const vec3 Camera::position() const
{
    return _target->position() + _distance;
}

vec3 Camera::up() const {
    vec4 up = inverse(orientation()) * vec4(0,1,0,1);
    return vec3(up);
}

vec3 Camera::forward() const {
    vec4 up = inverse(orientation()) * vec4(0,0,-1,1);
    return vec3(up);
}

vec3 Camera::right() const {
    vec4 up = inverse(orientation()) * vec4(1,0,0,1);
    return vec3(up);
}

void Camera::setViewportAspectRatio(float ratio)
{
    _viewportAspectRatio = ratio;
}

void Camera::offsetOrienatation(float upAngle, float rightAngle)
{
    _target->offsetLookAngle(upAngle, rightAngle);

    // YAY
    _distance = glm::rotate(Distance, _target->_verticalAngle, vec3(1,0,0));
    _distance = glm::rotate(_distance, _target->_horizontalAngle, vec3(0,1,0));
}