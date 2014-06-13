#pragma once

#include <list>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

#include <bullet/btBulletCollisionCommon.h>

#include "../Holder/Program.h"
#include "../Holder/Texture2.h"

struct ModelAsset {
    Program* program;
    Program* program2;

    GLuint vao;
    GLuint vao2;

    GLuint positionBuffer;
    GLuint normalBuffer;
    GLuint indexBuffer;
    GLuint uvBuffer;

    GLenum drawType;
    GLint drawStart;
    GLint drawCount;
};

struct Material {
    Texture2* texture;
    vec3 color;
    vec3 specularColor;
    float shininess;
};

struct ModelInstance {
    ModelAsset* asset;
    Material* material;
    mat4 transform;
    btCollisionObject* collisionObject;

    ModelInstance(std::list<ModelInstance*>* instances, btCollisionWorld* collisionWorld) : 
	_instances(instances),
	_collisionWorld(collisionWorld) 
    {
	_deathMark = false;
    }

    virtual ~ModelInstance() {
	_collisionWorld->removeCollisionObject(collisionObject);
    };

    virtual vec3 position() { return vec3(); };
    virtual void update(float time, float deltaT) = 0;
    virtual void collide(ModelInstance* other, vec3 pA, vec3 pB) = 0;

    virtual void markDeleted() { _deathMark = true; };
    virtual bool isMarkedDeleted() { return _deathMark; };

protected:
    std::list<ModelInstance*>* _instances;
    btCollisionWorld* _collisionWorld;

private:
    bool _deathMark;
};

struct Particle {
    ModelAsset* asset;
    Material* material;
    mat4 transform;

    float _creationTime;

    Particle(vec3 position, Material* mat, ModelAsset* a, float time) : 
        _position(position),
	_creationTime(time)
    {
	asset = a;
	material = mat;
	
	_scale = (rand() % 20) / 100.f;
	
	transform = glm::scale(mat4(), vec3(_scale, _scale, _scale));

	int x = rand() % 100 - 50;
	int y = rand() % 100 - 50;
	int z = rand() % 100 - 50;

	_direction = vec3(x, y, z) / 50.f;
	_velocity = rand() % 25 + 25;
    }

    void update(float time, float deltaT) {
        _position += deltaT * _direction * _velocity;
	transform = glm::translate(mat4(), _position);
	transform = glm::scale(transform, vec3(_scale, _scale, _scale));
    }

private:
    vec3 _position;
    vec3 _direction;
    float _velocity;
    float _scale;
};

struct Light {
    vec3 position;
    vec3 intensities;
    float attenuation;
    float ambientCoefficient;
};
