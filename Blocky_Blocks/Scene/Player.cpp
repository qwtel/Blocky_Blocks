#include "Player.h"

#include "Enemy.h"
#include "World.h"
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

static const float ShootDuration = 0.15f;

static const float MinVerticalAngle = -25.0f;
static const float MaxVerticalAngle = 65.0f;

static const float MoveSpeed = 16.0f; //units per second
static const float MoveDuration = 0.12f; //duration of animation

static const float JumpDuration = 0.5f; //duration of animation
static const float JumpHeight = 3.0f;

static const float FallSpeed = 32.f; //units per second

Player::Player(ModelAsset* ma, Material* mat, std::list<ModelInstance*>* instances, btCollisionWorld* collisionWorld) :
    ModelInstance(instances, collisionWorld),
    _position(-40, 20, 40),
    _horizontalAngle(135.0f),
    _verticalAngle(0.0f),
    _rotateDirection(),
    _isRotating(false),
    _isColliding(false),
    _isJumping(false),
    _isFalling(false),
    _shootStart(0.0f),
    _rotateAngle()
{
    asset = ma;
    material = mat;
    transform = glm::translate(mat4(), _position);

    btBoxShape* box = new btBoxShape(btVector3(1.2f, 1.2f, 1.2f));
    box->setMargin(0.f);

    collisionObject = new btCollisionObject();
    collisionObject->setUserPointer(this);
    collisionObject->setCollisionShape(box);
    collisionObject->getWorldTransform().setFromOpenGLMatrix(glm::value_ptr(transform));

    _instances->push_back(this);
    _collisionWorld->addCollisionObject(collisionObject);
}

Player::~Player(void)
{
}

vec3 Player::position()
{
    return _position;
}

void Player::_offsetPosition(vec3 offset)
{
    _position += offset;
}

void Player::update(float time, float deltaT)
{
    _time = time;
    _deltaT = deltaT;

    if (_isJumping) {
        float x = (time - _jumpStart) / JumpDuration;

        // change from 0..1 to -1..1, see below
        x = (x*2) - 1;

        // simpe "jump" function, see: https://www.wolframalpha.com/input/?i=-%28x%5E2%29+%2B+1%29
        float height = -(x*x) + 1;

        //printf("f(%f) = %f\n", x, height);

        _position.y = _jumpStartHeight + height * JumpHeight;
    }

    if (!_isColliding) {
        _position.y = _position.y - FallSpeed * deltaT;
    } else {
        _isJumping = false;
    }

    if (_isRotating) {
        if (_rotateStart + MoveDuration < time /*&& !_isJumping*/) {
            _isRotating = false;
            _isColliding = false;

            // TODO: this is not correct
            _rotateAngle += _rotateDirection * 90.0f;
        } else {
            _offsetPosition(MoveSpeed * deltaT * _moveDirection);
            float x = (time - _rotateStart) / MoveDuration;
            float rotateAngle = 90.0f * x;

            // this is necessary to rotate around the edge of the cube
            vec3 bla = vec3();
            if (_rotateDirection == LeftRotate) {
                bla = vec3(1, -1, 0);
            } else if (_rotateDirection == RightRotate) {
                bla = vec3(-1, -1, 0);
            } else if (_rotateDirection == ForwardRotate) {
                bla = vec3(0, -1, 1);
            } else if (_rotateDirection == BackwardRotate) {
                bla = vec3(0, -1, -1);
            }

            // translate to the position at the start of the rotation
            transform = glm::translate(mat4(), vec3(_rotateStartPosition.x , _position.y, _rotateStartPosition.z));

            // rotate according to the look direction at the start of the rotation
            transform = glm::rotate(transform, _rotateStartHorizontalAngle, vec3(0,1,0));

            // rotate cube around the edge
            transform = glm::translate(transform, bla);
            transform = glm::rotate(transform, rotateAngle, _rotateDirection);
            transform = glm::translate(transform, -bla);
        }
    } else {
        transform = glm::translate(mat4(), _position);

        // rotate according to the look direction
        transform = glm::rotate(transform, _horizontalAngle, vec3(0,1,0));
    }

    // rotate 90 degree-ish so textures look good
    transform = glm::rotate(transform, _rotateAngle.z, ZAxis); 
    transform = glm::rotate(transform, _rotateAngle.y, YAxis); 
    transform = glm::rotate(transform, _rotateAngle.x, XAxis); 

    collisionObject->getWorldTransform().setFromOpenGLMatrix(glm::value_ptr(transform));

    _isColliding = false;
}

void Player::_move(float time, vec3 direction, vec3 rotateDirection)
{
    if (!_isRotating) {
        _isRotating = true;
        _moveDirection = direction;
        _rotateDirection = rotateDirection;
        _rotateStart = time;
        _rotateStartPosition = _position;
        _rotateStartHorizontalAngle = _horizontalAngle;
    }
}

void Player::moveLeft(float time, float deltaT)
{
    vec3 direction = rotate(Left, _horizontalAngle, YAxis);
    _move(time, direction, LeftRotate);
}

void Player::moveRight(float time, float deltaT)
{
    vec3 direction = rotate(Right, _horizontalAngle, YAxis);
    _move(time, direction, RightRotate);
}

void Player::moveForward(float time, float deltaT)
{
    vec3 direction = rotate(Forward, _horizontalAngle, YAxis);
    _move(time, direction, ForwardRotate);
}

void Player::moveBackward(float time, float deltaT)
{
    vec3 direction = rotate(Backward, _horizontalAngle, YAxis);
    _move(time, direction, BackwardRotate);
}

void Player::jump(float time, float deltaT)
{
    if (!_isJumping) {
        _isJumping = true;
        _jumpStart = time;
        _jumpStartHeight = _position.y + FallSpeed * deltaT;
    }
}

void Player::offsetLookAngle(float upAngle, float rightAngle)
{
    _verticalAngle += upAngle;
    _horizontalAngle -= rightAngle;

    _horizontalAngle = fmodf(_horizontalAngle, 360.0f);

    //fmodf can return negative values, but this will make them all positive
    if(_horizontalAngle < 0.0f)
        _horizontalAngle += 360.0f;

    if(_verticalAngle > MaxVerticalAngle)
        _verticalAngle = MaxVerticalAngle;
    else if(_verticalAngle < MinVerticalAngle)
        _verticalAngle = MinVerticalAngle;
}

void Player::setLookAngle(float upAngle, float rightAngle)
{
    _verticalAngle = upAngle;
    _horizontalAngle = rightAngle;

    _horizontalAngle = fmodf(_horizontalAngle, 360.0f);

    //fmodf can return negative values, but this will make them all positive
    if(_horizontalAngle < 0.0f)
        _horizontalAngle += 360.0f;

    if(_verticalAngle > MaxVerticalAngle)
        _verticalAngle = MaxVerticalAngle;
    else if(_verticalAngle < -MaxVerticalAngle)
        _verticalAngle = -MaxVerticalAngle;
}

void Player::shoot(float time, float deltaT)
{
    if (_shootStart + ShootDuration < time) {
        _shootStart = time;
        Bullet* blt = new Bullet(asset, material, _instances, _collisionWorld, this);
        blt->shoot(_position, _horizontalAngle, _verticalAngle, _rotateDirection);

        // TODO make player smaller
    }
}

void Player::undoMovement()
{
    _moveDirection = _moveDirection * -1.0f;
    _rotateDirection = _rotateDirection * -1.0f;
    _rotateStartPosition = _position;
    _rotateStartHorizontalAngle = _horizontalAngle;
}

void Player::collide(ModelInstance* other, vec3 pA, vec3 pB) {
    if (Player* p = dynamic_cast<Player*>(other)) {
        undoMovement();
    } else if (Bullet* b = dynamic_cast<Bullet*>(other)) {
        if (b->_owner != this) {
            if (Enemy* e = dynamic_cast<Enemy*>(this)) {
                markDeleted();
            } else {
                setHit(true);
            }
        }
    } else if (World* w = dynamic_cast<World*>(other)) {
        //printf("Is this happening all the time??");
        _isColliding = true;

        if(pA.y < pB.y) {
            // ???
        } else {
            undoMovement();
        }

        //_position.y = _position.y + FallSpeed * _deltaT;

    } else {
        printf("Collision with unkown type\n");
        undoMovement();
    }
}
