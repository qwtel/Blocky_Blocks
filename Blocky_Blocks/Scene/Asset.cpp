#pragma once

#include <glm/glm.hpp>
using namespace glm;

#include <bullet/btBulletCollisionCommon.h>

#include "../Holder/Program.h"
#include "../Holder/Texture2.h"

struct ModelAsset {
    Program* program;
    //Texture2* texture;

    GLuint vao;
    GLuint positionBuffer;
    GLuint normalBuffer;
    GLuint indexBuffer;
    GLuint uvBuffer;

    // TODO: Remove this
    GLuint vbo;

    GLenum drawType;
    GLint drawStart;
    GLint drawCount;
};

struct Material {
    Texture2* texture;
    vec3 color;
    vec3 specularColor;
    float shininess;
};

struct ModelInstance {
    ModelAsset* asset;
    Material* material;
    mat4 transform;
    btCollisionObject* collisionObject;
};

struct Light {
    vec3 position;
    vec3 intensities;
    float attenuation;
    float ambientCoefficient;
};
