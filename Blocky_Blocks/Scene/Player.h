#pragma once

#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

#include "Asset.cpp"
#include "Bullet.h"

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
    void shoot(float time, float deltaT, std::list<Bullet*> *bullets);

    void update(float time, float deltaT);
    
    void offsetLookAngle(float upAngle, float rightAngle);

    float _horizontalAngle;
    float _verticalAngle;
private:
    vec3 _position;
    void _offsetPosition(vec3 offset);

    void _move(float time, vec3 direction, vec3 rotateDirection);

    // true while the cube is moving in a direction
    bool _isRotating;

    // x, y or z direction in which the cube is rotating
    vec3 _moveDirection;

    // x, y or z axis around which the cube is rotating while moving
    vec3 _rotateDirection;

    // time when a "move" started
    float _rotateStart;

    // true when the the cube is in the air
    bool _isJumping;

    // time when a "jump" started
    float _jumpStart;

    // the y coordinate at which the cube started jumping (so it can be reset at the end)
    float _jumpStartHeight;

    float _shootStart;
};

