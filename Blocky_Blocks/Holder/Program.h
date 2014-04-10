#pragma once
#pragma once

#include <functional>

#include "Shader.h"

class Program : public Holder
{
public:
    Program(Shader& vertexShader, Shader& fragmentShader);
    virtual ~Program(void);

    GLint attrib(const GLchar* name) const;
    GLint uniform(const GLchar* name) const;

private:
    static GLint checkedInvoke(const GLchar* name, const function<GLint(void)>& f);
};

