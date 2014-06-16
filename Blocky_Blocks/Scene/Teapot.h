#pragma once

#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
using namespace glm;

#include <bullet/btBulletCollisionCommon.h>

#include "Asset.cpp"

class Teapot : public ModelInstance
{
public:
    Teapot(ModelAsset* ma, Material* mat, btTriangleMesh* triMesh, std::list<ModelInstance*>* instances, btCollisionWorld* collisionWorld);
    virtual ~Teapot(void);
    virtual void collide(ModelInstance* other, vec3 pA, vec3 pB);
    virtual void update(float a, float b) {};
private:
};
