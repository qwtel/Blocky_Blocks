#include "Teapot.h"

Teapot::Teapot(ModelAsset* ma, Material* mat, btTriangleMesh* triMesh, std::list<ModelInstance*>* instances, btCollisionWorld* collisionWorld) :
    ModelInstance(instances, collisionWorld)
{
    asset = ma;
    transform = scale(translate(mat4(), vec3(0,12,0)),vec3(2,2,2));
    material = mat;

    btCollisionShape* collisionShape = new btBoxShape(btVector3(1,1,1));

    btTransform temp;
    temp.setFromOpenGLMatrix(glm::value_ptr(transform));

    collisionObject = new btCollisionObject();
    collisionObject->setUserPointer(this);
    collisionObject->setCollisionShape(collisionShape);
    collisionObject->setWorldTransform(temp);

    _instances->push_back(this);
    _collisionWorld->addCollisionObject(collisionObject);
}


Teapot::~Teapot(void)
{
}

void Teapot::collide(ModelInstance* other, vec3 pA, vec3 pB)
{
}
