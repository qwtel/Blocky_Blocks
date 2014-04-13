#include "Block.h"

#include "../External/Bitmap.h"
#include "../Holder/Texture2.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

Block::Block(Program& program)
    : SceneObject(program), _degreesRotated(0.0f)
{
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile("Blocky_Blocks/Texture/wooden-crate.jpg");
    bmp.flipVertically();
    Texture2* texture = new Texture2(bmp);

    glUniform1i(_program.uniform("tex"), 0); //set to 0 because the texture is bound to GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->object());

    // make and bind the VAO
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // make and bind the VBO
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    GLfloat vertexData[] = {
        //  X     Y     Z       U     V
        // bottom
        -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,

        // top
        -1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
        1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

        // front
        -1.0f,-1.0f, 1.0f,   1.0f, 0.0f,
        1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        1.0f,-1.0f, 1.0f,   0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,

        // back
        -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f, 1.0f,-1.0f,   0.0f, 1.0f,
        1.0f, 1.0f,-1.0f,   1.0f, 1.0f,

        // left
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,
        -1.0f,-1.0f,-1.0f,   0.0f, 0.0f,
        -1.0f,-1.0f, 1.0f,   0.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,   1.0f, 1.0f,
        -1.0f, 1.0f,-1.0f,   1.0f, 0.0f,

        // right
        1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
        1.0f,-1.0f,-1.0f,   1.0f, 0.0f,
        1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
        1.0f,-1.0f, 1.0f,   1.0f, 1.0f,
        1.0f, 1.0f,-1.0f,   0.0f, 0.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 1.0f
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

Block::~Block(void)
{
}

void Block::update(double deltaT)
{
     const GLfloat degreesPerSecond = 90.0f;
     _degreesRotated += degreesPerSecond * deltaT;

    //don't go over 360 degrees
    while(_degreesRotated > 360.0f) _degreesRotated -= 360.0f;
}

void Block::draw()
{
    mat4 modelMatrix = rotate(mat4(), _degreesRotated, vec3(0,1,0));
    glUniformMatrix4fv(_program.uniform("model"), 1, GL_FALSE, value_ptr(modelMatrix));

    // bind the program (the shaders)
    // glUseProgram(_program.object());

    // bind the VAO (the triangle)
    glBindVertexArray(_vao);

    // draw the VAO
    glDrawArrays(GL_TRIANGLES, 0, 6*2*3);

    // unbind the VAO (the triangle)
    glBindVertexArray(0);

    // unbind the program (the shaders)
    // glUseProgram(0);
}