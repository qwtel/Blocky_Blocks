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
    Program* shadowProgram;

    GLuint vao;
    GLuint vao2;
    GLuint shadowVao;

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
    hit = false;
    }

    virtual ~ModelInstance() {
	_collisionWorld->removeCollisionObject(collisionObject);
    };

    virtual vec3 position() { return vec3(); };
    virtual void update(float time, float deltaT) = 0;
    virtual void collide(ModelInstance* other, vec3 pA, vec3 pB) = 0;

    virtual void markDeleted() { _deathMark = true; };
    virtual bool isMarkedDeleted() { return _deathMark; };
    virtual void setHit(bool h) { hit = h;};
    virtual bool getHit() { return hit;};
protected:
    std::list<ModelInstance*>* _instances;
    btCollisionWorld* _collisionWorld;

private:
    bool _deathMark;
    bool hit;
};

struct Light {
    vec3 position;
    vec3 intensities;
    float attenuation;
    float ambientCoefficient;
    vec3 direction;
    float range;
};
