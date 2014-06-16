#include "Enemy.h"

// make a move approx. every x millisecondons
static const int Moviness = 500;

// 1 -> jump on every move, 10 -> jump approx. every 10th move, 0 -> divide by 0 exception
static const int Jumpiness = 10;

// 1 -> shoot on every move, 10 -> shoot approx. every 10th move
static const int Shootiness = 2;

// 10 -> will aim at max 10 degrees off target
static const int Aiminess = 10;

Enemy::Enemy(ModelAsset* ma, float time, Player* of, Material* mat, std::list<ModelInstance*>* instances, btCollisionWorld* collisionWorld) : 
    Player(ma, mat, instances, collisionWorld),
    _enemy(of),
    _lastMove(time),
    _waitTime(rand() % 2)
{
    if(time < 3){

        int posx = rand() % 47;
        int posz = rand() % 47;

        if(-6 < posx < 6 && -6 < posz < 6){posx += 20 + (rand() % 10 -5);}

        _position = vec3(posx, 30, posz); 
    }
    else{
        int posx = rand() % 94 - 47;
        int posz = rand() % 94 - 47;

        if(-6 < posx < 6 && -6 < posz < 6){posx += 20 + (rand() % 10 -5);}

        _position = vec3(posx, 30, posz); 
    }

    shooting = true;
    //printf("%f, %f, %f\n", _position.x, _position.y, _position.z);
    //color = vec3(255,153,153);
}

Enemy::~Enemy(void)
{
}


void Enemy::update(float time, float deltaT)
{
    if (time > _lastMove + _waitTime) {
        bool shouldJump = rand() % Jumpiness == 0;
        if (shouldJump) jump(time, deltaT);

        int dir = rand() % 4 + 1;
        if (dir == 1) {
            moveLeft(time, deltaT);
        } else if (dir == 2) {
            moveRight(time, deltaT);
        } else if (dir == 3) {
            moveForward(time, deltaT);
        } else if (dir == 4) {
            moveBackward(time, deltaT);
        } else {
            printf("something went wrong");
        }

        _lastMove = time;
        _waitTime = (rand() % Moviness + 1) / 1000.0f;

        bool shouldShoot = rand() % Shootiness == 0;
        if (shouldShoot && shooting) {
            shoot(time, deltaT);
        }

        vec3 direction = normalize(_position - _enemy->position());
        float zAngle = glm::angle(vec3(0,0,1), direction);
        zAngle = _jitter(zAngle);

        float yAngle = glm::angle(vec3(0,-1,0), direction);

        //printf("%f <-> %f\n", _verticalAngle, yAngle - 90);

        if (direction.x > 0) {
            setLookAngle(yAngle - 90, 180 + zAngle);
        } else {
            setLookAngle(yAngle - 90, 180 - zAngle);
        }
    }

    super::update(time, deltaT);
}

float Enemy::_jitter(float angle)
{
    float asdf = rand() % (Aiminess * 2) - Aiminess;
    return angle + asdf;
}

void Enemy::stopShooting(){

    shooting = false;   
}