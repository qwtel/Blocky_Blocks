#pragma once
#include "sceneobject.h"
class Block :
    public SceneObject
{
public:
    Block(void);
    virtual ~Block(void);

    virtual void update(double deltaT);
    virtual void draw();
};

