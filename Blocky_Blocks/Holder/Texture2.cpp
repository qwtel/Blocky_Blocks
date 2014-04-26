#include "Texture2.h"

static GLenum TextureFormatForBitmapFormat(tdogl::Bitmap::Format format)
{
    switch (format) {
    case tdogl::Bitmap::Format_Grayscale: return GL_LUMINANCE;
    case tdogl::Bitmap::Format_GrayscaleAlpha: return GL_LUMINANCE_ALPHA;
    case tdogl::Bitmap::Format_RGB: return GL_RGB;
    case tdogl::Bitmap::Format_RGBA: return GL_RGBA;
    default: throw std::runtime_error("Unrecognised Bitmap::Format");
    }
}

Texture2::Texture2(tdogl::Bitmap bmp, GLint minMagFilter, GLint wrapMode) : 
    Holder(),
    _width((GLfloat)bmp.width()),
    _height((GLfloat)bmp.height())
{
    glGenTextures(1, &_object);
    glBindTexture(GL_TEXTURE_2D, _object);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minMagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexImage2D(GL_TEXTURE_2D,
        0, 
        TextureFormatForBitmapFormat(bmp.format()),
        (GLsizei)bmp.width(), 
        (GLsizei)bmp.height(),
        0, 
        TextureFormatForBitmapFormat(bmp.format()), 
        GL_UNSIGNED_BYTE, 
        bmp.pixelBuffer());

    glBindTexture(GL_TEXTURE_2D, 0);
}


Texture2::~Texture2(void)
{
    glDeleteTextures(1, &_object);
}

GLfloat Texture2::originalWidth() const
{
    return _width;
}

GLfloat Texture2::originalHeight() const
{
    return _height;
}