#pragma once

#include <list>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
using namespace glm;

#include <bullet/btBulletCollisionCommon.h>

#include "Player.h"
#include "Bullet.h"

class Enemy : public Player
{
public:
    Enemy(ModelAsset* ma, float time, Player* of, std::list<Bullet*>* bullets, Material* mat, btCollisionWorld* collisionWorld);
    virtual ~Enemy(void);

    virtual void update(float t, float deltaT);

private:
    typedef Player super;

    Player* _enemy;
    std::list<Bullet*>* _bullets;

    // time between to moves
    float _waitTime;

    // last time a move was made
    float _lastMove;

    float _jitter(float angle);

    btCollisionWorld* _collisionWorld;
};

