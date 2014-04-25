#include "Enemy.h"

Enemy::Enemy(ModelAsset* ma, float time, Player* of) : 
    Player(ma),
	_enemy(of),
    _lastMove(time),
    _waitTime(rand() % 2)
{
    _position = vec3(rand() % 50 + 1, 0, rand() % 50 + 1);
    printf("%f, %f, %f\n", _position.x, _position.y, _position.z);
}

Enemy::~Enemy(void)
{
}


void Enemy::update(float time, float deltaT)
{
    if (time > _lastMove + _waitTime) {
		bool shouldJump = rand() % 2 == 1;
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
			printf("fucked up");
		}

		_lastMove = time;
		_waitTime = (rand() % 1000 + 1) / 1000.0f;
    }

    super::update(time, deltaT);
}