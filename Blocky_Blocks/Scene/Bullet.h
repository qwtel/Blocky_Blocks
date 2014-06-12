#pragma once

#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

#include <bullet/btBulletCollisionCommon.h>

#include "Asset.cpp"
#include "Player.h"

class Bullet : public ModelInstance
{
public:
    Bullet(ModelAsset* ma, Material* mat, std::list<ModelInstance*>* instances, btCollisionWorld* collisionWorld, Player* owner);
    ~Bullet(void);
    vec3 position();
    void shoot(vec3 from, float x, float y, vec3 rotateDirection);
    vec3 _posi;

    Player* _owner;

    virtual void update(float time, float deltaT);
    virtual void collide(ModelInstance* other, vec3 pA, vec3 pB);
private:
    vec3 _direction;
    float _velocity;
    vec3 _rotateDirection;
    float _verticalAngle;
    float _horizontalAngle;
    float _rotateAngle;
};

