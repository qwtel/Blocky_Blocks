#include "SceneObject.h"


SceneObject::SceneObject(Program& program) 
    : _program(program), _vao(0), _vbo(0)
{
}


SceneObject::~SceneObject(void)
{
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
}

void SceneObject::update(double deltaT)
{
}


void SceneObject::draw()
{
}