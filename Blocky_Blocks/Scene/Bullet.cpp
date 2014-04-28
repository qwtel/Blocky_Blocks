#include "Bullet.h"

static const vec3 XAxis = vec3(1,0,0);
static const vec3 YAxis = vec3(0,1,0);
static const vec3 ZAxis = vec3(0,0,1);

static const vec3 Forward = ZAxis;
static const vec3 ForwardRotate = XAxis;
static const vec3 Backward = -Forward;
static const vec3 BackwardRotate = -ForwardRotate;

static const vec3 Left = XAxis;
static const vec3 LeftRotate = -ZAxis;
static const vec3 Right = -Left;
static const vec3 RightRotate = -LeftRotate;

Bullet::Bullet(ModelAsset* ma, Material* mat) :
    _posi(),
    _velocity(50.0f), // todo
    _direction(),
    _rotateAngle()
{
    _rotateAngle = rand() % 360 + 1;
    // printf("%f\n", _rotateAngle);
    asset = ma;
    transform = mat4();
    material = mat;
}


Bullet::~Bullet(void)
{
}

void Bullet::shoot(vec3 from, float x, float y, vec3 rotateDirection)
{
    _posi = from;
    _horizontalAngle = x;
    _verticalAngle = y;
    _rotateDirection = rotateDirection;
    _direction = glm::rotate(vec3(0,0,1), y, vec3(1,0,0));
    _direction = glm::rotate(_direction, x, vec3(0,1,0));

    // TODO: should have the correct transform matrix by now
}

void Bullet::update(float time, float deltaT)
{
    _posi= _posi + _direction * _velocity * deltaT;

    transform = glm::translate(mat4(), _posi);
    transform = glm::rotate(transform, _horizontalAngle, vec3(0,1,0));
    transform = glm::rotate(transform, _verticalAngle, vec3(1,0,0));
    transform = glm::scale(transform, vec3(0.5f));

    if (_rotateDirection != vec3(0,0,0)) {
        static const float degreePerSecond = 360.0f;

        _rotateAngle += deltaT * degreePerSecond;
        _rotateAngle = fmodf(_rotateAngle, 360.0f);

        if(_rotateAngle < 0.0f)
            _rotateAngle += 360.0f;

        transform = glm::rotate(transform, _rotateAngle, _rotateDirection);
    }

}