#pragma once

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <list>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "External/Bitmap.h"
#include "Holder/Shader.h"
#include "Holder/Program.h"
#include "Holder/Texture2.h"
#include "Scene/Camera.h"
#include "Scene/Player.h"
#include "Scene/Bullet.h"
#include "Scene/Enemy.h"

#include "Scene/Mesh.h"
#include "Scene/Asset.cpp"

const vec2 SCREEN_SIZE(800, 600);
const vec2 CENTER = SCREEN_SIZE * 0.5f;

GLFWwindow* window;
Camera* camera;

ModelAsset gWoodenCrate;
ModelAsset gWorld;
std::list<ModelInstance> gInstances;
Light gLight;
Player* player;
std::list<Bullet*> bullets;
std::list<Enemy*> enemies;

GLuint positionBuffer;
GLuint normalBuffer;
GLuint indexBuffer;
GLuint uvBuffer;

Assimp::Importer* importer;

vector<Mesh*> meshes;

GLFWwindow* openWindow(int width, int height);
void Update(double time, double deltaT);
void Draw();
void DrawInstance(const ModelInstance& inst);
void cleanup();

static void LoadWoodenCrateAsset();
static Program* LoadShaders();
static Texture2* LoadTexture();
static void CreateInstances();
static void ImportScene(const std::string& pFile);
static void LoadWorld();
static void CreateWorldInstance();
static Material* GiveMaterial(vec3 color, string texPath);

static const std::string Assets(const std::string& path) {
    return "Assets/" + path;
}

int main() 
{
    // (1) init everything you need
    window = openWindow(SCREEN_SIZE.x, SCREEN_SIZE.y);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    bool running = true;
    double lastTime = 0;

    //import cube from file
    ImportScene(Assets("Models/cube.model")); 

    // initialise the gWoodenCrate asset
    LoadWoodenCrateAsset();
    // create all the instances in the 3D scene based on the gWoodenCrate asset
    //CreateInstances();

    //initialise world
    ImportScene(Assets("Models/world.model"));
    LoadWorld();
    CreateWorldInstance();

    player = new Player(&gWoodenCrate, GiveMaterial(vec3(132,213,219),"Texture/noise.png"));
    camera = new Camera(player);
    //player->setCamera(camera);

    double time = glfwGetTime();
    static const int NumEnemies = 10;
    for (int i = 0; i < NumEnemies; i++) {
        Enemy* enemy = new Enemy(&gWoodenCrate, time, player, &bullets, GiveMaterial(vec3(255,153,153),"Texture/noise.png"));
        enemies.push_back(enemy);
    }

    //camera->setPosition(vec3(0,0,4));
    camera->setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    gLight.position = camera->position();
    gLight.position.y = gLight.position.y + 15.0f;
    gLight.intensities = vec3(1, 1, 1) * 0.9f;
    gLight.attenuation = 0.0005f;
    gLight.ambientCoefficient = 0.75f;

    while (running && !glfwWindowShouldClose(window))
    {
        // (2) clear the frame and depth buffer
        vec3 bg = vec3(225,209,244) / 255.0f;
        glClearColor(bg.r, bg.g, bg.b, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // (3) compute the frame time delta
        time = glfwGetTime();
        double deltaT = time - lastTime;
        lastTime = time;

        // (4) react to user input
        glfwPollEvents();
        running = !glfwGetKey(window, GLFW_KEY_ESCAPE);

        // (5) Update all game components
        Update(time, deltaT);

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

void Update(double time, double deltaT) 
{
    float deltaTf = float(deltaT);
    float timef = float(time);

    if(glfwGetKey(window, 'S')){
        player->moveBackward(timef, deltaTf);
    } else if(glfwGetKey(window, 'W')){
        player->moveForward(timef, deltaTf);
    }

    if(glfwGetKey(window, 'A')){
        player->moveLeft(timef, deltaTf);
    } else if(glfwGetKey(window, 'D')){
        player->moveRight(timef, deltaTf);
    }

    if(glfwGetKey(window, GLFW_KEY_SPACE)) {
        player->jump(timef, deltaTf);
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
        player->shoot(timef, deltaTf, &bullets);
    }

    // TODO: All these could be in one list
    player->update(timef, deltaTf);

    // since enemies can shoot, update them before the bullets 
    std::list<Enemy*>::const_iterator it2;
    for(it2 = enemies.begin(); it2 != enemies.end(); ++it2) {
        (*it2)->update(timef, deltaTf);
    }

    std::list<Bullet*>::const_iterator bullet_it;
    for(bullet_it = bullets.begin(); bullet_it != bullets.end();) {
        Bullet* blt = *bullet_it;

        if (blt->_posi.x > 100 || blt->_posi.y > 100 || blt->_posi.z > 100 ||
            blt->_posi.x < -100 || blt->_posi.y < -100 || blt->_posi.z < -100)
        {
            bullet_it = bullets.erase(bullet_it);
            delete blt;
        } else {
            blt->update(timef, deltaTf);
            ++bullet_it;
        }
    }

    //rotate camera based on mouse movement
    const float mouseSensitivity = 0.1;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float diffX = mouseX - CENTER.x;
    float diffY = mouseY - CENTER.y;

    camera->offsetOrienatation(mouseSensitivity * diffY, mouseSensitivity * diffX);

    glfwSetCursorPos(window, CENTER.x, CENTER.y); //reset the mouse, so it doesn't go out of the window
}

void Draw() 
{
    // one shader to rule them all...
    glUseProgram(player->asset->program->object());

    // TODO: All these could be in one list
    DrawInstance(*player);

    std::list<Bullet*>::const_iterator bullet_it;
    for(bullet_it = bullets.begin(); bullet_it != bullets.end(); ++bullet_it) {
        Bullet* blt = *bullet_it;
        DrawInstance(*blt);
    }

    std::list<ModelInstance>::const_iterator it;
    for(it = gInstances.begin(); it != gInstances.end(); ++it){
        DrawInstance(*it);
    }

    std::list<Enemy*>::const_iterator it2;
    for(it2 = enemies.begin(); it2 != enemies.end(); ++it2){
        DrawInstance(*(*it2));
    }

    glUseProgram(0);
}

void DrawInstance(const ModelInstance& inst)
{
    ModelAsset* asset = inst.asset;
    Material* material = inst.material;
    Program* program = asset->program;

    // bind the shaders
    // NOTE: since we only have one program we already did this..
    //glUseProgram(program->object());

    // bind VAO
    glBindVertexArray(asset->vao);

    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->texture->object());

    // set the shader uniforms
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, glm::value_ptr(camera->matrix()));
    glUniformMatrix4fv(program->uniform("model"), 1, GL_FALSE, glm::value_ptr(inst.transform));

    glUniform3fv(program->uniform("cameraPosition"), 1, glm::value_ptr(camera->position()));

    glUniform3fv(program->uniform("light.position"), 1, glm::value_ptr(gLight.position));
    glUniform3fv(program->uniform("light.intensities"), 1, glm::value_ptr(gLight.intensities));
    glUniform1f(program->uniform("light.attenuation"), gLight.attenuation);
    glUniform1f(program->uniform("light.ambientCoefficient"), gLight.ambientCoefficient);

    glUniform3fv(program->uniform("material.color"), 1, glm::value_ptr(material->color)); 
    glUniform3fv(program->uniform("material.specularColor"), 1, glm::value_ptr(material->specularColor));
    glUniform1f(program->uniform("material.shininess"), material->shininess);

    glUniform1i(program->uniform("tex"), 0);


    glDrawElements(asset->drawType, asset->drawCount, GL_UNSIGNED_INT, 0);


    // unbind vao and texture
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // unbind shaders
    // NOTE: since we only have one program we dont need to do this
    // glUseProgram(0);
}

void cleanup()
{
    glDeleteVertexArrays(1, &gWoodenCrate.vao);
    glDeleteVertexArrays(1, &gWoodenCrate.vbo);
    glDeleteBuffers(1, &positionBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteBuffers(1, &normalBuffer);
    glDeleteBuffers(1, &uvBuffer);

    importer->FreeScene();
    delete importer;
    for(unsigned int i = 0; i < meshes.size(); i++)
    {
        delete meshes[i];
    }


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
    glfwWindowHint(GLFW_SAMPLES, 2);

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
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(Assets("Texture/noise.png"));
    bmp.flipVertically();
    return new Texture2(bmp);
}

static void LoadWoodenCrateAsset()
{
    gWoodenCrate.program = LoadShaders();
    gWoodenCrate.drawType = GL_TRIANGLES;
    gWoodenCrate.drawStart = 0;
    gWoodenCrate.drawCount = meshes[0]->numIndices;
    //gWorld.texture = LoadTexture();

    // create and bind VAO
    glGenVertexArrays(1, &gWoodenCrate.vao);
    glBindVertexArray(gWoodenCrate.vao);

    glGenBuffers(1, &gWoodenCrate.positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, meshes[0]->numVertices* 3 * sizeof(float), meshes[0]->vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gWoodenCrate.program->attrib("vert"));
    glVertexAttribPointer(gWoodenCrate.program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &gWoodenCrate.normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, meshes[0]->numVertices* 3 * sizeof(float), meshes[0]->normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gWoodenCrate.program->attrib("vertNormal"));
    glVertexAttribPointer(gWoodenCrate.program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);

    glGenBuffers(1, &gWoodenCrate.uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, meshes[0]->numVertices* 3 * sizeof(float), meshes[0]->textureCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gWoodenCrate.program->attrib("vertTexCoord"));
    glVertexAttribPointer(gWoodenCrate.program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE,  3*sizeof(float), 0);

    glGenBuffers(1, &gWoodenCrate.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWoodenCrate.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[0]->numIndices * sizeof(unsigned int), meshes[0]->indices, GL_STATIC_DRAW);

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static Program* LoadShaders() {
    Shader* vertexShader = new Shader(Assets("Shader/lightingVertexShader.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/lightingFragmentshader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Material* GiveMaterial(vec3 color, string texPath)
{

    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(Assets(texPath));
    bmp.flipVertically();
    Texture2* tex = new Texture2(bmp);

    Material* m = new Material();
    m->texture = tex;
    m->color = color;
    m->shininess = 80.0f;
    m->specularColor = vec3(1,1,1);
    return m;
}


static void CreateInstances() {
    vec3 color = vec3(132,213,219);

    Material* m = GiveMaterial(color,"Texture/noise.png");

    ModelInstance i;
    i.asset = &gWoodenCrate;
    i.transform = translate(mat4(), vec3(0,-4,0)) * scale(mat4(), vec3(1,2,1));
    i.material = m;
    gInstances.push_back(i);

    ModelInstance hLeft;
    hLeft.asset = &gWoodenCrate;
    hLeft.transform = translate(mat4(), vec3(-8,0,0)) * scale(mat4(), vec3(1,6,1));
    i.material = m;
    gInstances.push_back(hLeft);

    ModelInstance hRight;
    hRight.asset = &gWoodenCrate;
    hRight.transform = translate(mat4(), vec3(-4,0,0)) * scale(mat4(), vec3(1,6,1));
    i.material = m;
    gInstances.push_back(hRight);

    ModelInstance hMid;
    hMid.asset = &gWoodenCrate;
    hMid.transform = translate(mat4(), vec3(-6,0,0)) * scale(mat4(), vec3(2,1,0.8));
    i.material = m;
    gInstances.push_back(hMid);
}

static void ImportScene(const std::string& pFile) {

    importer = new Assimp::Importer();
    const aiScene* scene = importer->ReadFile(pFile,
        aiProcess_Triangulate               |
        aiProcess_GenSmoothNormals          |
        aiProcess_JoinIdenticalVertices     |
        aiProcess_ImproveCacheLocality      |
        aiProcess_SortByPType               |
        aiProcess_FlipUVs);

    for(unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        Mesh* tempMesh = new Mesh(scene->mMeshes[i]->mNumVertices,
            scene->mMeshes[i]->mVertices,
            scene->mMeshes[i]->mNormals,
            scene->mMeshes[i]->mNumFaces,
            scene->mMeshes[i]->mFaces,
            scene->mMeshes[i]->mTextureCoords[0]);

        meshes.push_back(tempMesh);
    }
}

static void CreateWorldInstance()
{
    ModelInstance world;
    world.asset = &gWorld;
    world.transform = translate(mat4(), vec3(0,-1,0)) * scale(mat4(), vec3(1, 1, 1));
    Material* m = GiveMaterial(vec3(192,158,233),"Texture/tilewall10.jpg");
    //world.color = vec3(182,148,233);
    world.material = m;
    gInstances.push_back(world);
}

static void LoadWorld()
{
    gWorld.program = LoadShaders();
    gWorld.drawType = GL_TRIANGLES;
    gWorld.drawStart = 0;
    gWorld.drawCount = meshes[1]->numIndices;
    //gWorld.texture = LoadTexture();

    // create and bind VAO
    glGenVertexArrays(1, &gWorld.vao);
    glBindVertexArray(gWorld.vao);

    glGenBuffers(1, &gWorld.positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gWorld.positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, meshes[1]->numVertices* 3 * sizeof(float), meshes[1]->vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gWorld.program->attrib("vert"));
    glVertexAttribPointer(gWorld.program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &gWorld.normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gWorld.normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, meshes[1]->numVertices* 3 * sizeof(float), meshes[1]->normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gWorld.program->attrib("vertNormal"));
    glVertexAttribPointer(gWorld.program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);

    glGenBuffers(1, &gWorld.uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gWorld.uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, meshes[1]->numVertices* 3 * sizeof(float), meshes[1]->textureCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gWorld.program->attrib("vertTexCoord"));
    glVertexAttribPointer(gWorld.program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE,  3*sizeof(float), 0);

    glGenBuffers(1, &gWorld.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWorld.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[1]->numIndices * sizeof(unsigned int), meshes[1]->indices, GL_STATIC_DRAW);

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
