#include "Triangle.h"

#include "../External/Bitmap.h"
#include "../Holder/Texture2.h"

Triangle::Triangle(Program& program) :
    SceneObject(program)
{
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile("Blocky_Blocks/Texture/hazard.png");
    bmp.flipVertically();
    Texture2* texture = new Texture2(bmp);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->object());
    glUniform1i(_program.uniform("tex"), 0); //set to 0 because the texture is bound to GL_TEXTURE0

    // make and bind the VAO
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // make and bind the VBO
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    // Put the three triangle verticies into the VBO
    GLfloat vertexData[] = {
        //  X     Y     Z       U     V
        0.0f, 0.8f, 0.0f,   0.5f, 1.0f,
        -0.8f,-0.8f, 0.0f,   0.0f, 0.0f,
        0.8f,-0.8f, 0.0f,   1.0f, 0.0f,
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    // connect the xyz to the "vert" attribute of the vertex shader
    glEnableVertexAttribArray(_program.attrib("vert"));
    glVertexAttribPointer(_program.attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), NULL);

    // connect the uv coords to the "vertTexCoord" attribute of the vertex shader
    glEnableVertexAttribArray(_program.attrib("vertTexCoord"));
    glVertexAttribPointer(_program.attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE, 5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    // unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Triangle::~Triangle(void)
{
}

void Triangle::update(double deltaT)
{
}

void Triangle::draw()
{
    // bind the program (the shaders)
    glUseProgram(_program.object());

    // bind the VAO (the triangle)
    glBindVertexArray(_vao);

    // draw the VAO
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glUseProgram(0);
}