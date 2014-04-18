#include "Player.h"

Player::Player(ModelAsset* ma) :
    _position(vec3()),
    _rotateAngle(0.0f),
    _lookAngle(0.0f),
    _rotateDirection(),
    _isRotating(false)
{
    asset = ma;
    transform = mat4();
}

Player::~Player(void)
{
}

vec3 Player::position()
{
    return _position;
}

void Player::_offsetPosition(vec3 offset)
{
    _position += offset;
    //_camera->offsetPosition(offset);
}

void Player::update(float time, float deltaT)
{
    static const float moveSpeed = 8.0; //units per second
    static const double moveDuration = 0.25; //duration of animation

    // this is not good
    transform = glm::translate(mat4(), _position);
    transform = glm::rotate(transform, _lookAngle, vec3(0, 1, 0));

    if (_isRotating) {
        if (_rotateStart + moveDuration < time) {
            _isRotating = false;
        } else {
            _rotateAngle = 90.0f * ((time - _rotateStart) / moveDuration);
            _offsetPosition(moveSpeed * deltaT * _moveDirection);

            // this is not good
            transform = glm::rotate(transform, _rotateAngle, _rotateDirection);
        }
    }
}

void Player::_move(float time, vec3 direction, vec3 rotateDirection)
{
    if (!_isRotating) {
        _isRotating = true;
        _moveDirection = direction;
        _rotateDirection = rotateDirection;
        _rotateStart = time;
    }
}

void Player::moveLeft(float time, float deltaT)
{
    // TODO: make dependent on camera
    _move(time, vec3(1, 0, 0), vec3(0, 0, -1));
}

void Player::moveRight(float time, float deltaT)
{
    // TODO: make dependent on camera
    _move(time, vec3(-1, 0, 0), vec3(0, 0, 1));
}

void Player::moveForward(float time, float deltaT)
{
    // TODO: make dependent on camera
    _move(time, vec3(0, 0, 1), vec3(1, 0, 0));
}

void Player::moveBackward(float time, float deltaT)
{
    // TODO: make dependent on camera
    _move(time, vec3(0, 0, -1), vec3(-1, 0, 0));
}

void Player::jump(float time, float deltaT)
{
    // TODO
}

void Player::offsetLookAngle(float lookAngle)
{
    _lookAngle += lookAngle;
    if(_lookAngle < 0.0f)
        _lookAngle += 360.0f;
}

float Player::getLookAngle()
{
    return _lookAngle;
}

/*
void Player::setCamera(Camera* camera)
{
    _camera = camera;
}
*/