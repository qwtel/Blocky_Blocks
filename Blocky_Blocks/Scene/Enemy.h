#pragma once

#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

#include "Player.h"
#include "Bullet.h"

class Enemy : public Player
{
public:
    Enemy(ModelAsset* ma, float time, Player* of);
    virtual ~Enemy(void);

    virtual void update(float t, float deltaT);

private:
    typedef Player super;

    Player* _enemy;

    // time between to moves
    float _waitTime;

    // last time a move was made
    float _lastMove;
};

