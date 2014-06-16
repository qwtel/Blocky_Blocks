#include "Teapot.h"

Teapot::Teapot(ModelAsset* ma, Material* mat, btTriangleMesh* triMesh, std::list<ModelInstance*>* instances, btCollisionWorld* collisionWorld) :
    ModelInstance(instances, collisionWorld)
{
    asset = ma;
    transform = scale(translate(mat4(), vec3(0,12,0)),vec3(2,2,2));
    material = mat;
    _instances->push_back(this);
}


Teapot::~Teapot(void)
{
}

void Teapot::collide(ModelInstance* other, vec3 pA, vec3 pB)
{
}
