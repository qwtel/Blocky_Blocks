#pragma once
#include <GL/glew.h>
class SceneObject
{
public:
    SceneObject(void);
    virtual ~SceneObject(void);

    virtual void update(double deltaT);
    virtual void draw();
};

