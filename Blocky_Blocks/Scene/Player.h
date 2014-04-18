#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

#include "Source.cpp"

class Player : public ModelInstance
{
public:
    Player(ModelAsset* ma);
    ~Player(void);
    vec3 position();

    void moveLeft(float time, float deltaT);
    void moveRight(float time, float deltaT);
    void moveForward(float time, float deltaT);
    void moveBackward(float time, float deltaT);
    void jump(float time, float deltaT);

    void update(float time, float deltaT);
    
    void offsetLookAngle(float upAngle, float rightAngle);
    float _horizontalAngle;
    float _verticalAngle;
private:
    vec3 _position;
    void _offsetPosition(vec3 offset);

    void _move(float time, vec3 direction, vec3 rotateDirection);

    bool _isRotating;
    vec3 _moveDirection;
    vec3 _rotateDirection;
    float _rotateStart;

    bool _isJumping;
    float _jumpStart;
    float _jumpStartHeight;

};

