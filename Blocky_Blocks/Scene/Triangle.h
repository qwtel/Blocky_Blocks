#pragma once
#include "sceneobject.h"
class Triangle : public SceneObject
{
public:
    Triangle(void);
    virtual ~Triangle(void);

    virtual void update(double deltaT);
    virtual void draw();
private:
    GLuint vertexbuffer;
    GLfloat* data;
};

