#include "World.h"

World::World(ModelAsset* ma, Material* mat, btTriangleMesh* triMesh, std::list<ModelInstance*>* instances, btCollisionWorld* collisionWorld) :
    ModelInstance(instances, collisionWorld)
{
    asset = ma;
    transform = translate(mat4(), vec3(0,-1,0));
    material = mat;

    btCollisionShape* collisionShape = new btBvhTriangleMeshShape(triMesh, false);

    btTransform temp;
    temp.setFromOpenGLMatrix(glm::value_ptr(transform));

    collisionObject = new btCollisionObject();
    collisionObject->setUserPointer(this);
    collisionObject->setCollisionShape(collisionShape);
    collisionObject->setWorldTransform(temp);

    _instances->push_back(this);
    _collisionWorld->addCollisionObject(collisionObject);
}


World::~World(void)
{
}

void World::collide(ModelInstance* other, vec3 pA, vec3 pB)
{
}
