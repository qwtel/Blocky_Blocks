#pragma once

#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

using namespace glm;

#include <bullet/btBulletCollisionCommon.h>

#include "Asset.cpp"

class Player : public ModelInstance
{
public:
    Player(ModelAsset* ma, Material* mat, std::list<ModelInstance*>* instances, btCollisionWorld* collisionWorld);

    virtual ~Player(void);
    virtual vec3 position();

    virtual void moveLeft(float time, float deltaT);
    virtual void moveRight(float time, float deltaT);
    virtual void moveForward(float time, float deltaT);
    virtual void moveBackward(float time, float deltaT);
    virtual void jump(float time, float deltaT);
    virtual void shoot(float time, float deltaT);

    virtual void update(float time, float deltaT);
    
    virtual void setLookAngle(float upAngle, float rightAngle);
    virtual void offsetLookAngle(float upAngle, float rightAngle);

    float actualHorizontalAngle;
    float _horizontalAngle;
    float _verticalAngle;

    virtual void collide(ModelInstance* other, vec3 pA, vec3 pB);

protected:
    vec3 _position;

private:
    void _offsetPosition(vec3 offset);
    vec3 _rotateAngle;

    // helper method for directional movement and animation
    void _move(float time, vec3 direction, vec3 rotateDirection);

    // true while the cube is moving in a direction
    bool _isRotating;

    // x, y or z direction in which the cube is rotating
    vec3 _moveDirection;

    // x, y or z axis around which the cube is rotating while moving
    vec3 _rotateDirection;

    float _time;

    float _deltaT;

    // time when a "move" started
    float _rotateStart;

    float _rotateDuration;

    // true when the the cube is in the air
    bool _isJumping;

    bool _isFalling;

    // time when a "jump" started
    float _jumpStart;

    // the y coordinate at which the cube started jumping (so it can be reset at the end)
    float _jumpStartHeight;

    // there is only a limited number of shots allowed per second
    // this is the start time of a shot to enforce the limit
    float _shootStart;

    vec3 _rotateStartPosition;
    float _rotateStartHorizontalAngle;

    bool _isColliding;

    void undoMovement();
};

