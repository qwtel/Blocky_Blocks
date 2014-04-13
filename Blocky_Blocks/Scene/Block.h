#pragma once

#include <stdlib.h>

#include "sceneobject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class Block : public SceneObject
{
public:
    Block(Program& program);
    virtual ~Block(void);

    virtual void update(double deltaT);
    virtual void draw();

private:
    float _degreesRotated;
};

