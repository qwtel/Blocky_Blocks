#include "Program.h"

Program::Program(Shader* vertexShader, Shader* fragmentShader) : 
    Holder()
{
    // Create program
    _object = glCreateProgram();

    if (_object == 0)
    {
        fprintf(stderr, "Failed to create shader program\n");
        system("PAUSE");
        exit(-1);
    }

    // Attach shader to program
    glAttachShader(_object, vertexShader->object());
    glAttachShader(_object, fragmentShader->object());

    // Link programs
    glLinkProgram(_object);

    // check for errors
    GLint succeded;
    glGetProgramiv(_object, GL_LINK_STATUS, &succeded);

    if (!succeded)
    {
        string msg("Program linking failure: ");

        GLint logSize;
        glGetProgramiv(_object, GL_INFO_LOG_LENGTH, &logSize);

        auto log = new char[logSize];
        glGetProgramInfoLog(_object, logSize, NULL, log);

        msg += log;
        delete[] log;

        glDeleteProgram(_object);
        _object = 0;

        fprintf(stderr, "%s\n", msg);
        system("PAUSE");
        exit(-1);
    }
}

Program::~Program(void)
{
    if (_object != 0)
    {
        glDeleteProgram(_object);
    }
}

GLint Program::attrib(const GLchar* name) const
{
    return checkedInvoke(name, [=]() -> GLint {
        return glGetAttribLocation(_object, name);
    });
}

GLint Program::uniform(const GLchar* name) const 
{
    return checkedInvoke(name, [=]() -> GLint {
        return glGetUniformLocation(_object, name);
    });
}

GLint Program::checkedInvoke(const GLchar* name, const function<GLint(void)>& f)
{
    if(!name) 
    {
        fprintf(stderr, "name was NULL\n");
        system("PAUSE");
        exit(-1);
    }

    GLint attrib = f();

    if(attrib == -1)
    {
        fprintf(stderr, "Program name not found: %s\n", name);
        system("PAUSE");
        exit(-1);
    }

    return attrib;
}
