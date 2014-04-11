#pragma once

#include <GL/glew.h>

#include "../Holder/Program.h"

class SceneObject
{
public:
    SceneObject(Program& program);
    virtual ~SceneObject(void);

    virtual void update(double deltaT);
    virtual void draw();
protected:
    Program& _program;
    GLuint _vao;
    GLuint _vbo;
};

