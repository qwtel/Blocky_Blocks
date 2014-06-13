#pragma once 

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

#include "./Asset.cpp"

struct Particle {
    ModelAsset* asset;
    Material* material;
    mat4 transform;

    float _creationTime;
    float _distance;

    Particle(vec3 position, Material* mat, ModelAsset* a, float time, int mass) : 
        _position(position),
	_creationTime(time),
	_distance(0.f)
    {
	asset = a;
	material = mat;
	
	_scale = (rand() % mass) / 100.f;
	
	transform = glm::scale(mat4(), vec3(_scale, _scale, _scale));

	int x = rand() % 100 - 50;
	int y = rand() % 100 - 50;
	int z = rand() % 100 - 50;

	_direction = vec3(x, y, z) / 50.f;
	_velocity = rand() % 25 + 25;
    }

    void update(float time, float deltaT) {
        _position += deltaT * _direction * _velocity;
        _distance += deltaT * _direction.length() * _velocity;
	transform = glm::translate(mat4(), _position);
	transform = glm::scale(transform, vec3(_scale, _scale, _scale));
    }

private:
    vec3 _position;
    vec3 _direction;
    float _velocity;
    float _scale;
};
