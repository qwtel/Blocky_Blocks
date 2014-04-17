#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <list>
	
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/GLU.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "External/Bitmap.h"
#include "Holder/Shader.h"
#include "Holder/Program.h"
#include "Holder/Texture2.h"
#include "Scene/Camera.h"

const vec2 SCREEN_SIZE(800, 600);
const vec2 CENTER = SCREEN_SIZE * 0.5f;

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

GLFWwindow* window;
Camera camera;
ModelAsset gWoodenCrate;
std::list<ModelInstance> gInstances;
GLfloat gDegreesRotated = 0.0f;

GLFWwindow* openWindow(int width, int height);
void Update(double deltaT);
void Draw();
void DrawInstance(const ModelInstance& inst);
void cleanup();

static void LoadWoodenCrateAsset();
static Program* LoadShaders();
static Texture2* LoadTexture();
static void CreateInstances();

int main() 
{
    // (1) init everything you need
    window = openWindow(SCREEN_SIZE.x, SCREEN_SIZE.y);

    camera = Camera();
    camera.setPosition(vec3(0,0,4));
    camera.setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    bool running = true;
    double lastTime = 0;

    // initialise the gWoodenCrate asset
    LoadWoodenCrateAsset();

    // create all the instances in the 3D scene based on the gWoodenCrate asset
    CreateInstances();

    while (running && !glfwWindowShouldClose(window))
    {
        // (2) clear the frame and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // (3) compute the frame time delta
        double time = glfwGetTime();
        double deltaT = time - lastTime;
        lastTime = time;

        // (4) react to user input
        glfwPollEvents();
        running = !glfwGetKey(window, GLFW_KEY_ESCAPE);

        // (5) Update all game components
        Update(deltaT);

        // (6) draw all game components
        Draw();

        // (6.5) swap buffers
        glfwSwapBuffers(window);

        // (7) check for errors
        GLenum errCode;
        if ((errCode = glGetError()) != GL_NO_ERROR) {
            fprintf(stderr, "OpenGL Error: %i\n", errCode);
        }
    }

    // (8) clean up!
    cleanup();

    glfwTerminate();
}

void Update(double deltaT) 
{
    //move position of camera based on WASD keys
    const float moveSpeed = 4.0; //units per second

    float deltaTf = float(deltaT);

    if(glfwGetKey(window, 'S')){
        camera.offsetPosition(deltaTf * moveSpeed * -camera.forward());
    } else if(glfwGetKey(window, 'W')){
        camera.offsetPosition(deltaTf * moveSpeed * camera.forward());
    }

    if(glfwGetKey(window, 'A')){
        camera.offsetPosition(deltaTf * moveSpeed * -camera.right());
    } else if(glfwGetKey(window, 'D')){
        camera.offsetPosition(deltaTf * moveSpeed * camera.right());
    }

    if(glfwGetKey(window, 'Z')){
        camera.offsetPosition(deltaTf * moveSpeed * -vec3(0,1,0));
    } else if(glfwGetKey(window, 'X')){
        camera.offsetPosition(deltaTf * moveSpeed * vec3(0,1,0));
    }

    //rotate camera based on mouse movement
    const float mouseSensitivity = 0.1;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float diffX = mouseX - CENTER.x;
    float diffY = mouseY - CENTER.y;

    camera.offsetOrienatation(mouseSensitivity * diffY, mouseSensitivity * diffX);

    glfwSetCursorPos(window, CENTER.x, CENTER.y); //reset the mouse, so it doesn't go out of the window

    //rotate the first instance in `gInstances`
    const GLfloat degreesPerSecond = 180.0f;
    gDegreesRotated += deltaT * degreesPerSecond;
    while(gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;

    // meh..
    gInstances.front().transform = rotate(mat4(), gDegreesRotated, vec3(0,1,0));
}

void Draw() 
{
    std::list<ModelInstance>::const_iterator it;
    for(it = gInstances.begin(); it != gInstances.end(); ++it){
        DrawInstance(*it);
    }
}

void DrawInstance(const ModelInstance& inst)
{
    ModelAsset* asset = inst.asset;
    Program* program = asset->program;

    //bind the shaders
    glUseProgram(program->object());

    //set the shader uniforms
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, value_ptr(camera.matrix()));
    glUniformMatrix4fv(program->uniform("model"), 1, GL_FALSE, value_ptr(inst.transform));
    glUniform1i(program->uniform("tex"), 0);

    //bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, asset->texture->object());

    //bind VAO and draw
    glBindVertexArray(asset->vao);
    glDrawArrays(asset->drawType, asset->drawStart, asset->drawCount);

    //unbind everything
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void cleanup()
{
}

GLFWwindow* openWindow(int width, int height) 
{
    // (1) init glfw
    if (!glfwInit()) 
    {
        fprintf(stderr, "Failed to init glfw");
        system("PAUSE");
        exit(-1);
    }
	

    // (2) set window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // (3) open window
    GLFWwindow* window = glfwCreateWindow(width, height, "Hello, CGUE!", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "Failed to open window"); 
        system("PAUSE");
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE; 

    // (4) init glew
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize glew");
        system("PAUSE");
        exit(-1);
    }

    glGetError();

    fprintf(stdout, "Opened OpenGL Window.\n");

    // glClearColor(0.14f, 0.25f, 0.43f, 0.0f);
    glViewport(0, 0, width, height);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPos(window, double(width) / 2.0, double(height) / 2.0);

    return window;
}

static Texture2* LoadTexture()
{
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile("Blocky_Blocks/Texture/wooden-crate.jpg");
    bmp.flipVertically();
    return new Texture2(bmp);
}

static void LoadWoodenCrateAsset()
{
    gWoodenCrate.program = LoadShaders();
    gWoodenCrate.drawType = GL_TRIANGLES;
    gWoodenCrate.drawStart = 0;
    gWoodenCrate.drawCount = 6*2*3;
    gWoodenCrate.texture = LoadTexture();
    glGenBuffers(1, &gWoodenCrate.vbo);
    glGenVertexArrays(1, &gWoodenCrate.vao);

    // bind the VAO
    glBindVertexArray(gWoodenCrate.vao);

    // bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.vbo);

    // TODO: Load vertexData from file
    // Make a cube out of triangles (two triangles per side)
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
    glEnableVertexAttribArray(gWoodenCrate.program->attrib("vert"));
    glVertexAttribPointer(gWoodenCrate.program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), NULL);

    // connect the uv coords to the "vertTexCoord" attribute of the vertex shader
    glEnableVertexAttribArray(gWoodenCrate.program->attrib("vertTexCoord"));
    glVertexAttribPointer(gWoodenCrate.program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_TRUE,  5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

    // unbind the VAO
    glBindVertexArray(0);
}

static Program* LoadShaders() {
    Shader* vertexShader = new Shader("Blocky_Blocks/Shader/cameraVertexShader.vert", GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader("Blocky_Blocks/Shader/textureFragmentshader.frag", GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static void CreateInstances() {
    ModelInstance dot;
    dot.asset = &gWoodenCrate;
    dot.transform = mat4();
    gInstances.push_back(dot);

    ModelInstance i;
    i.asset = &gWoodenCrate;
    i.transform = translate(mat4(), vec3(0,-4,0)) * scale(mat4(), vec3(1,2,1));
    gInstances.push_back(i);

    ModelInstance hLeft;
    hLeft.asset = &gWoodenCrate;
    hLeft.transform = translate(mat4(), vec3(-8,0,0)) * scale(mat4(), vec3(1,6,1));
    gInstances.push_back(hLeft);

    ModelInstance hRight;
    hRight.asset = &gWoodenCrate;
    hRight.transform = translate(mat4(), vec3(-4,0,0)) * scale(mat4(), vec3(1,6,1));
    gInstances.push_back(hRight);

    ModelInstance hMid;
    hMid.asset = &gWoodenCrate;
    hMid.transform = translate(mat4(), vec3(-6,0,0)) * scale(mat4(), vec3(2,1,0.8));
    gInstances.push_back(hMid);
}
