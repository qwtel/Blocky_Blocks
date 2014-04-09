#pragma once

#include <stdlib.h>

#include "sceneobject.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

class Block : public SceneObject
{
public:
    Block(GLuint matrixID);
    virtual ~Block(void);

    virtual void update(double deltaT);
    virtual void draw(mat4 vp);

private:
    GLuint vertexbuffer;
    GLuint colorbuffer;

    mat4 modelMatrix;
    GLuint matrixID_;
    float gDegreesRotated;
};

