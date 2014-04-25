#include "Enemy.h"

// make a move approx. every x millisecondons
static const int Moviness = 500;

// 1 -> jump on every move, 10 -> jump approx. every 10th move, 0 -> divide by 0 exception
static const int Jumpiness = 10;

// 1 -> shoot on every move, 10 -> shoot approx. every 10th move
static const int Shootiness = 10;

// 10 -> will aim at max 10 degrees off target
static const int Aiminess = 10;

Enemy::Enemy(ModelAsset* ma, float time, Player* of, std::list<Bullet*>* bullets) : 
    Player(ma),
	_enemy(of),
    _lastMove(time),
    _waitTime(rand() % 2),
	_bullets(bullets)
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
			printf("fucked up");
		}

		_lastMove = time;
		_waitTime = (rand() % Moviness + 1) / 1000.0f;

		bool shouldShoot = rand() % Shootiness == 0;
		if (shouldShoot) {
			shoot(time, deltaT, _bullets);
        }


		vec3 direction = normalize(_position - _enemy->position());
        float zAngle = glm::angle(vec3(0,0,1), direction);
		float angle = _jitter(zAngle);

		if (direction.x > 0) {
			setLookAngle(_verticalAngle, 180 + angle);
        } else {
			setLookAngle(_verticalAngle, 180 - angle);
        }

		// TODO: Aim vertically
    }

    super::update(time, deltaT);
}

float Enemy::_jitter(float angle)
{
	float asdf = rand() % (Aiminess * 2) - Aiminess;
	return angle + asdf;
}