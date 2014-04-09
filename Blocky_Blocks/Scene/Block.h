#pragma once
#include <stdlib.h>
#include "sceneobject.h"
class Block : public SceneObject
{
public:
    Block(void);
    virtual ~Block(void);

    virtual void update(double deltaT);
    virtual void draw();
private:
    GLuint vertexbuffer;
    GLuint colorbuffer;
};

