#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include "../Holder/Program.h"
#include "../Holder/Texture2.h"

struct ModelAsset {
    Program* program;
    Texture2* texture;
    GLuint vbo;
    GLuint vao;
    GLenum drawType;
    GLint drawStart;
    GLint drawCount;
};

struct ModelInstance {
    ModelAsset* asset;
    mat4 transform;
};

struct Light {
    vec3 position;
    vec3 intensities; //a.k.a. the color of the light
};