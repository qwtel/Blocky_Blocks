#include "Camera.h"

static const float MaxVerticalAngle = 85.0f; //must be less than 90 to avoid gimbal lock

Camera::Camera(void) :
    _position(0.0f, 0.0f, 1.0f),
    _horizontalAngle(0.0f),
    _verticalAngle(0.0f),
    _fieldOfView(50.0f), 
    _viewportAspectRatio(4.0f/3.0f), 
    _nearPlane(0.1f), 
    _farPlane(100.0f)
{
}

Camera::~Camera(void)
{
}

mat4 Camera::matrix() const {
    mat4 camera = perspective(_fieldOfView, _viewportAspectRatio, _nearPlane, _farPlane);
    camera *= orientation();
    camera = translate(camera, -_position);
    return camera;
}

mat4 Camera::orientation() const {
    mat4 orientation;
    orientation = rotate(orientation, _verticalAngle, vec3(1,0,0));
    orientation = rotate(orientation, _horizontalAngle, vec3(0,1,0));
    return orientation;
}

const vec3& Camera::position() const
{
    return _position;
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

void Camera::setPosition(const vec3& position)
{
    _position = position;
}

void Camera::setViewportAspectRatio(float ratio)
{
    _viewportAspectRatio = ratio;
}

void Camera::offsetPosition(const vec3& offset)
{
    _position += offset;
}

void Camera::offsetOrienatation(float upAngle, float rightAngle)
{
    _horizontalAngle += rightAngle;
    _verticalAngle += upAngle;
    normalizeAngles();
}

void Camera::normalizeAngles() {
    _horizontalAngle = fmodf(_horizontalAngle, 360.0f);

    //fmodf can return negative values, but this will make them all positive
    if(_horizontalAngle < 0.0f)
        _horizontalAngle += 360.0f;

    if(_verticalAngle > MaxVerticalAngle)
        _verticalAngle = MaxVerticalAngle;
    else if(_verticalAngle < -MaxVerticalAngle)
        _verticalAngle = -MaxVerticalAngle;
}