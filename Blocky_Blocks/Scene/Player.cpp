#include "Player.h"

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

static const float MaxVerticalAngle = 85.0f;

Player::Player(ModelAsset* ma) :
    _position(vec3()),
    _horizontalAngle(0.0f),
    _verticalAngle(0.0f),
    _rotateDirection(),
    _isRotating(false),
    _isJumping(false),
    _shootStart(0.0f),
    _rotateAngle()
{
    asset = ma;
    transform = mat4();
    color = vec3(132,213,219);
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
    static const float MoveSpeed = 8.0; //units per second
    static const float MoveDuration = 0.25; //duration of animation

    static const float JumpDuration = 0.5; //duration of animation
    static const float JumpHeight = 3.0f;

    if (_isJumping) {
        if (_jumpStart + JumpDuration < time) {
            _isJumping = false;
            _isRotating = false;
            _position.y = _jumpStartHeight; // XXX
        } else {
            float x = (time - _jumpStart) / JumpDuration;

            // change from 0..1 to -1..1, see below
            x = (x*2) - 1;

            // simpe "jump" function, see: https://www.wolframalpha.com/input/?i=-%28x%5E2%29+%2B+1%29
            float height = -(x*x) + 1;

            //printf("f(%f) = %f\n", x, height);

            _position.y = _jumpStartHeight + height * JumpHeight;
        }
    }


    if (_isRotating) {
        if (_rotateStart + MoveDuration < time && !_isJumping) { // can't stop while jumping
            _isRotating = false;

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
        _jumpStartHeight = _position.y;
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
    else if(_verticalAngle < -MaxVerticalAngle)
        _verticalAngle = -MaxVerticalAngle;
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

void Player::shoot(float time, float deltaT, std::list<Bullet*> *bullets)
{
    static const float ShootDuration = 0.1; // 10 shots per second

    if (_shootStart + ShootDuration < time) {
        _shootStart = time;
        // TODO: allocation during game is not so good
        Bullet* blt = new Bullet(asset, color);
        blt->shoot(_position, _horizontalAngle, _verticalAngle, _rotateDirection);
        bullets->push_back(blt);

        // todo make player smaller
    }
}