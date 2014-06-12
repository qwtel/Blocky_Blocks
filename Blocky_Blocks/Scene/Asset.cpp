#pragma once

#include <list>

#include <glm/glm.hpp>
using namespace glm;

#include <bullet/btBulletCollisionCommon.h>

#include "../Holder/Program.h"
#include "../Holder/Texture2.h"

struct ModelAsset {
    Program* program;
    //Texture2* texture;

    GLuint vao;
    GLuint positionBuffer;
    GLuint normalBuffer;
    GLuint indexBuffer;
    GLuint uvBuffer;

    // TODO: Remove this
    GLuint vbo;

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

struct Light {
    vec3 position;
    vec3 intensities;
    float attenuation;
    float ambientCoefficient;
};
