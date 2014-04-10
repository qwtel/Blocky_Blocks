#pragma once

#include <GL\glew.h>

class Holder
{
public:
    Holder(void);
    virtual ~Holder(void);
    GLuint object() const;
protected:
    GLuint _object;
};

