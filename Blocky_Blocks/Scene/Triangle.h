#pragma once

#include "SceneObject.h"

class Triangle : 
    public SceneObject
{
public:
    Triangle(Program& program);
    virtual ~Triangle(void);

    virtual void update(double deltaT);
    virtual void draw();
private:
};

