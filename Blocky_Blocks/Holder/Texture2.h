#pragma once

#include <GL/glew.h>

#include "../External/Bitmap.h"

#include "Holder.h"

class Texture2 : public Holder
{
public:
    Texture2(tdogl::Bitmap bmp, 
        GLint minMagFiler = GL_LINEAR,
        GLint wrapMode = GL_REPEAT);
    virtual ~Texture2(void);

    GLfloat originalWidth() const;
    GLfloat originalHeight() const;
private:
    GLfloat _width;
    GLfloat _height;
};