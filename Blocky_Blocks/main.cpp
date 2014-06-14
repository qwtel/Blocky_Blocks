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

#include <bullet/btBulletCollisionCommon.h>

#include "External/Bitmap.h"
#include "Holder/Shader.h"
#include "Holder/Program.h"
#include "Holder/Texture2.h"

#include "Scene/Camera.h"
#include "Scene/Player.h"
#include "Scene/Bullet.h"
#include "Scene/Enemy.h"
#include "Scene/World.h"

#include "Scene/Mesh.h"

#include "Scene/Asset.cpp"
#include "Scene/Particle.cpp"

const vec2 SCREEN_SIZE(800, 600);
const vec2 CENTER = SCREEN_SIZE * 0.5f;

static const int TimeToLive = 50;
static const int NumEnemies = 0;

GLFWwindow* window;
Camera* camera;

ModelAsset gWoodenCrate;
ModelAsset gWorld;
std::list<ModelInstance*> gInstances;
Light gLight;
Player* player;
World* world;
int currentEnemies = 0;

GLuint positionBuffer;
GLuint normalBuffer;
GLuint indexBuffer;
GLuint uvBuffer;

Assimp::Importer* importer;
vector<Mesh*> meshes;

btCollisionWorld* collisionWorld = 0;

std::list<Particle*> particles;

Program* instancingProgram;
Program* instancingProgram2;
GLuint instancingVao;
GLuint instancingVao2;
GLuint instancingVbo;
GLuint instancingColorVbo;

GLuint fuckvao;

GLFWwindow* openWindow(int width, int height);
void Update(double time, double deltaT);
void Draw();
void DrawInstance(const ModelInstance& inst);
void DrawContour(const ModelInstance& inst);
void DrawParticles(mat4* data, vec3* colors, int size);
void DrawParticlesContour(mat4* data, vec3* colors, int size);
void DrawInstanceDepth(const ModelInstance& inst);

void cleanup();

static void LoadWoodenCrateAsset();
static Program* LoadShaders();
static Program* LoadShaders2();
static Program* LoadDepthShaders();
static Program* LoadFuckShaders();

static Texture2* LoadTexture();
static void ImportScene(const std::string& pFile);
static void LoadWorldAsset();
static void CreateWorldInstance();
static Material* GiveMaterial(vec3 color, string texPath);
static btTriangleMesh* giveTriangleMesh(const struct aiMesh *pAIMesh);

static const std::string Assets(const std::string& path) {
    return "Assets/" + path;
}

GLuint FramebufferName;
GLuint depthTexture;
Program* depthProgram;
GLuint depthMatrix;
GLuint shadowMappingVao;


GLuint vertexbuffer;
Program* fuckProgram;

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

    // initialise bullet
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // SimpleBroadphase is a brute force alternative, performing N^2 aabb overlap tests
    btSimpleBroadphase*	broadphase = new btSimpleBroadphase;

    collisionWorld = new btCollisionWorld(dispatcher, broadphase, collisionConfiguration);

    // initialise world
    ImportScene(Assets("Models/world.model"));
    LoadWorldAsset();
    CreateWorldInstance();

    player = new Player(&gWoodenCrate, GiveMaterial(vec3(132,213,219),"Texture/noise.png"), &gInstances, collisionWorld);

    camera = new Camera(player);

    double time = glfwGetTime();


    //camera->setPosition(vec3(0,0,4));
    camera->setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    gLight.position = vec3(0,30,0);
    //gLight.position.y = gLight.position.y + 15.0f;
    gLight.intensities = vec3(1, 1, 1) * 0.9f;
    gLight.attenuation = 0.0005f;
    gLight.ambientCoefficient = 0.75f;
    //gLight.direction=normalize(player->position()-camera->position());
    gLight.direction = vec3(0,-1,0);
    gLight.range = 100;


    glGenVertexArrays(1, &shadowMappingVao);
    glBindVertexArray(shadowMappingVao);

    depthProgram = LoadDepthShaders();
    depthMatrix = depthProgram->uniform("depthMVP");

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);


    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenVertexArrays(1, &fuckvao);
    glBindVertexArray(fuckvao);

    fuckProgram = LoadFuckShaders();

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    while (running && !glfwWindowShouldClose(window))
    {
        //printf("%i\n", particles.size());
        //move light
        /* gLight.position = player->position();
        gLight.position.y = gLight.position.y + 5;
        gLight.position.z = gLight.position.z + 5;
        gLight.direction=normalize(vec3(player->position().x-camera->position().x,0,player->position().z-camera->position().z));*/

        //spawn enemies
        for (int i = 0; i < NumEnemies - currentEnemies; i++) {
            Enemy* enemy = new Enemy(&gWoodenCrate, time, player, GiveMaterial(vec3(255,153,153),"Texture/noise.png"), &gInstances, collisionWorld);
            currentEnemies++;
        }

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

    collisionWorld->performDiscreteCollisionDetection();

    int numManifolds = collisionWorld->getDispatcher()->getNumManifolds();
    for (int i=0;i<numManifolds;i++)
    {
        btPersistentManifold* contactManifold =  collisionWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject* obA = contactManifold->getBody0();
        const btCollisionObject* obB = contactManifold->getBody1();
        ModelInstance* pA = (ModelInstance*) obA->getUserPointer();
        ModelInstance* pB = (ModelInstance*) obB->getUserPointer();

        int numContacts = contactManifold->getNumContacts();
        for (int j=0;j<numContacts;j++)
        {
            btManifoldPoint& pt = contactManifold->getContactPoint(j);
            if (pt.getDistance() < 0.f)
            {
                const btVector3& ptA = pt.getPositionWorldOnA();
                const btVector3& ptB = pt.getPositionWorldOnB();
                //const btVector3& normalOnB = pt.m_normalWorldOnB;

                vec3 pointA = vec3(ptA.getX(), ptA.getY(), ptA.getZ());
                vec3 pointB = vec3(ptB.getX(), ptB.getY(), ptB.getZ());

                pA->collide(pB, pointA, pointB);
                pB->collide(pA, pointB, pointA);
            }
        }
    }

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
        player->shoot(timef, deltaTf);
    }

    std::list<ModelInstance*>::const_iterator it;
    for(it = gInstances.begin(); it != gInstances.end();) {
        ModelInstance* instance = *it;
        instance->update(timef, deltaTf);


        if (instance->isMarkedDeleted()) {
            printf("Deleted something %s\n", typeid(*instance).name());
            if(typeid(*instance) == typeid(Enemy)){
                currentEnemies--;
            }

            if (dynamic_cast<Bullet*>(instance)) {
                for (int i = 0; i < 50; i++) {
                    Particle* part = new Particle(instance->position(), instance->material, instance->asset, timef, 10);
                    particles.push_back(part);
                }
            } else if (dynamic_cast<Player*>(instance)) {
                for (int i = 0; i < 250; i++) {
                    Particle* part = new Particle(instance->position(), instance->material, instance->asset, timef, 33);
                    particles.push_back(part);
                }
            }

            it = gInstances.erase(it);
            delete instance;
        } else {
            ++it;
        }
    }

    // update "particles"
    std::list<Particle*>::const_iterator it2;
    for (it2 = particles.begin(); it2 != particles.end();) {

        Particle* part = *it2;
        part->update(timef, deltaTf);

        if (part->_distance > TimeToLive) {
            it2 = particles.erase(it2);
            delete part;
        } else {
            ++it2;
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



    std::list<ModelInstance*>::const_iterator it;

    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,1024,1024);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(depthProgram->object());

    for(it = gInstances.begin(); it != gInstances.end(); ++it){
        DrawInstanceDepth(*(*it));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glViewport(0,0,SCREEN_SIZE.x,SCREEN_SIZE.y);
    glViewport(0,0,256,256);

    // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glBindVertexArray(fuckvao);

    // Use our shader
    glUseProgram(fuckProgram->object());

    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    // Set our "renderedTexture" sampler to user Texture Unit 0
    glUniform1i(fuckProgram->uniform("texture"), 3);

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
        );

    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, 6); // 3 indices starting at 0 -> 1 triangle

    glDisableVertexAttribArray(0);

    glEnable(GL_CULL_FACE);
    glViewport(0,0,SCREEN_SIZE.x,SCREEN_SIZE.y);

    // clear the frame and depth buffer
    //vec3 bg = vec3(225,209,244) / 255.0f;
    //glClearColor(bg.r, bg.g, bg.b, 1);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // cel shader
    glUseProgram(player->asset->program->object());
    glCullFace(GL_BACK);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    for(it = gInstances.begin(); it != gInstances.end(); ++it){
    DrawInstance(*(*it));
    }

    // contour shader
    glUseProgram(player->asset->program2->object());
    glCullFace(GL_FRONT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    for(it = gInstances.begin(); it != gInstances.end(); ++it){
    DrawContour(*(*it));
    }

    // "particle" shader

    const int size = particles.size();
    if (size > 0) {
    mat4* data = new mat4[size];
    vec3* colors = new vec3[size];

    std::list<Particle*>::const_iterator it;
    int n = 0;
    for(it = particles.begin(); it != particles.end(); ++it) {
    Particle* p = (*it);
    data[n] = mat4(p->transform);
    colors[n] = vec3(p->material->color);
    n++;
    }

    // cel shader
    glUseProgram(instancingProgram->object());
    glCullFace(GL_BACK);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    DrawParticles(data, colors, size);

    // contour shader
    glUseProgram(instancingProgram2->object());
    glCullFace(GL_FRONT);
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    DrawParticlesContour(data, colors, size);

    delete data;
    delete colors;
    }

    // clear
    //glUseProgram(0);
}

void DrawInstanceDepth(const ModelInstance& inst){

    glBindVertexArray(shadowMappingVao);

    //glm::vec3 lightInvDir = glm::vec3(0.5,1,0);

    glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 200.f);
    glm::mat4 depthViewMatrix = glm::lookAt(vec3(0, 10, 0), player->position(), glm::vec3(0,1,0));

    //glm::mat4 depthProjectionMatrix = glm::ortho<float>(-100,100,-100,100,-100,100);
    //glm::mat4 depthViewMatrix = glm::lookAt(vec3(0,0,0), glm::vec3(0,0,0), glm::vec3(0,1,0));

    glm::mat4 depthModelMatrix = inst.transform;
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
    glUniformMatrix4fv(depthMatrix, 1, GL_FALSE, &depthMVP[0][0]);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, inst.asset->indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, inst.asset->positionBuffer);
    glEnableVertexAttribArray(depthProgram->attrib("vert"));
    glVertexAttribPointer(depthProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawElements(inst.asset->drawType, inst.asset->drawCount, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(depthProgram->attrib("vert"));

}

void DrawParticles(mat4* data, vec3* colors, int size)
{
    glBindBuffer(GL_ARRAY_BUFFER, instancingVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * size, &data[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, instancingColorVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * size, &colors[0], GL_DYNAMIC_DRAW);

    Particle* p = particles.front();

    ModelAsset* asset = p->asset;
    Material* material = p->material;
    Program* program = instancingProgram;

    // bind VAO
    glBindVertexArray(instancingVao);

    // set the shader uniforms
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, glm::value_ptr(camera->matrix()));
    glUniform3fv(program->uniform("cameraPosition"), 1, glm::value_ptr(camera->position()));

    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->texture->object());
    glUniform1i(program->uniform("tex"), 0);

    glUniform3fv(program->uniform("light.position"), 1, glm::value_ptr(gLight.position));
    glUniform3fv(program->uniform("light.intensities"), 1, glm::value_ptr(gLight.intensities));
    glUniform1f(program->uniform("light.attenuation"), gLight.attenuation);
    glUniform1f(program->uniform("light.ambientCoefficient"), gLight.ambientCoefficient);

    //glUniform3fv(program->uniform("material.color"), 1, glm::value_ptr(material->color)); 
    glUniform3fv(program->uniform("material.specularColor"), 1, glm::value_ptr(material->specularColor));
    glUniform1f(program->uniform("material.shininess"), material->shininess);

    glDrawElementsInstanced(asset->drawType, asset->drawCount, GL_UNSIGNED_INT, 0, size);

    // unbind vao, vbo and texture
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DrawParticlesContour(mat4* data, vec3* colors, int size)
{
    glBindBuffer(GL_ARRAY_BUFFER, instancingVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * size, &data[0], GL_DYNAMIC_DRAW);

    Particle* p = particles.front();

    ModelAsset* asset = p->asset;
    //Material* material = p->material;
    Program* program = instancingProgram2;

    // bind VAO
    glBindVertexArray(instancingVao2);

    // set the shader uniforms
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, glm::value_ptr(camera->matrix()));

    glDrawElementsInstanced(asset->drawType, asset->drawCount, GL_UNSIGNED_INT, 0, size);

    // unbind vao, vbo and texture
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DrawInstance(const ModelInstance& inst)
{
    ModelAsset* asset = inst.asset;
    Material* material = inst.material;
    Program* program = asset->program;

    glm::vec3 lightInvDir = glm::vec3(0.5,1,0);

    /*glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
    glm::mat4 depthViewMatrix = glm::lookAt(gLight.position, gLight.position-lightInvDir, glm::vec3(0,1,0));*/

    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10,10,-10,10,-10,20);
    glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0,0,0), glm::vec3(0,1,0));

    glm::mat4 depthModelMatrix = inst.transform;
    glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

    glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0, 
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
        );

    glm::mat4 depthBiasMVP = biasMatrix*depthMVP;

    // bind VAO
    glBindVertexArray(asset->vao);

    // set the shader uniforms
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, glm::value_ptr(camera->matrix()));
    glUniform3fv(program->uniform("cameraPosition"), 1, glm::value_ptr(camera->position()));

    glUniformMatrix4fv(program->uniform("model"), 1, GL_FALSE, glm::value_ptr(inst.transform));

    glUniformMatrix4fv(program->uniform("depthBias"), 1, GL_FALSE, &depthBiasMVP[0][0]);
    /*   glUniformMatrix4fv(program->uniform("view"), 1, GL_FALSE, glm::value_ptr(camera->viewMatrix()));
    glUniform3fv(program->uniform("lightInvDir"), 1, glm::value_ptr(lightInvDir));*/

    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->texture->object());
    glUniform1i(program->uniform("tex"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(program->uniform("shadowMap"), 1);

    glUniform3fv(program->uniform("light.position"), 1, glm::value_ptr(gLight.position));
    glUniform3fv(program->uniform("light.intensities"), 1, glm::value_ptr(gLight.intensities));
    glUniform1f(program->uniform("light.attenuation"), gLight.attenuation);
    glUniform1f(program->uniform("light.ambientCoefficient"), gLight.ambientCoefficient);
    glUniform3fv(program->uniform("light.direction"), 1, glm::value_ptr(gLight.direction));
    glUniform1f(program->uniform("light.range"), gLight.range);

    glUniform3fv(program->uniform("material.color"), 1, glm::value_ptr(material->color)); 
    glUniform3fv(program->uniform("material.specularColor"), 1, glm::value_ptr(material->specularColor));
    glUniform1f(program->uniform("material.shininess"), material->shininess);

    glDrawElements(asset->drawType, asset->drawCount, GL_UNSIGNED_INT, 0);

    // unbind vao and texture
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, 1);
}

void DrawContour(const ModelInstance& inst) 
{
    ModelAsset* asset = inst.asset;
    Program* program = asset->program2;

    // bind VAO
    glBindVertexArray(asset->vao2);

    // set the shader uniforms
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, glm::value_ptr(camera->matrix()));
    glUniformMatrix4fv(program->uniform("model"), 1, GL_FALSE, glm::value_ptr(inst.transform));

    glDrawElements(asset->drawType, asset->drawCount, GL_UNSIGNED_INT, 0);

    // unbind vao and texture
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void cleanup()
{
    glDeleteVertexArrays(1, &gWoodenCrate.vao);
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

static Program* LoadShaders() 
{
    Shader* vertexShader = new Shader(Assets("Shader/shadowVertexShader.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/shadowCelShader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Program* LoadShaders2() 
{
    Shader* vertexShader = new Shader(Assets("Shader/vertexshader.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/fragmentshader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Program* LoadShadersI() 
{
    Shader* vertexShader = new Shader(Assets("Shader/instancingShader.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/instancingCelShader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Program* LoadShaders2I() 
{
    Shader* vertexShader = new Shader(Assets("Shader/instancingShader2.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/fragmentshader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}
static Program* LoadDepthShaders() 
{
    Shader* vertexShader = new Shader(Assets("Shader/depthShader.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/depthShader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Program* LoadFuckShaders() 
{
    Shader* vertexShader = new Shader(Assets("Shader/fuck.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/fuck.frag").c_str(), GL_FRAGMENT_SHADER);
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
        Mesh* tempMesh = new Mesh(scene->mMeshes[i],
            scene->mMeshes[i]->mNumVertices,
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
    Material* m = GiveMaterial(vec3(192,158,233),"Texture/noise.png");
    btTriangleMesh* triMesh = giveTriangleMesh(meshes[1]->mesh);
    world = new World(&gWorld, m, triMesh, &gInstances, collisionWorld);
}

static btTriangleMesh* giveTriangleMesh(const struct aiMesh *pAIMesh)
{
    btTriangleMesh* triMesh = new btTriangleMesh();

    aiFace *pAIFace;
    for ( int y = 0; y < pAIMesh->mNumFaces; y++ )
    {
        pAIFace = &pAIMesh->mFaces[y];

        if ( pAIFace->mNumIndices != 3 )
        {
            continue;
        }

        aiVector3D v1 = pAIMesh->mVertices[pAIFace->mIndices[0]];
        aiVector3D v2 = pAIMesh->mVertices[pAIFace->mIndices[1]];
        aiVector3D v3 = pAIMesh->mVertices[pAIFace->mIndices[2]];

        btVector3 btV1(v1.x,v1.y,v1.z);
        btVector3 btV2(v2.x,v2.y,v2.z);
        btVector3 btV3(v3.x,v3.y,v3.z);

        triMesh->addTriangle(btV1,btV2,btV3);
    }

    return triMesh;
}

static void LoadWoodenCrateAsset()
{
    gWoodenCrate.drawType = GL_TRIANGLES;
    gWoodenCrate.drawStart = 0;
    gWoodenCrate.drawCount = meshes[0]->numIndices;

    // create and bind VAO
    gWoodenCrate.program = LoadShaders();

    glGenVertexArrays(1, &gWoodenCrate.vao);
    glBindVertexArray(gWoodenCrate.vao);

    glGenBuffers(1, &gWoodenCrate.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWoodenCrate.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[0]->numIndices * sizeof(unsigned int), meshes[0]->indices, GL_STATIC_DRAW);

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

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // create and bind VAO for drawing contours
    gWoodenCrate.program2 = LoadShaders2();

    glGenVertexArrays(1, &gWoodenCrate.vao2);
    glBindVertexArray(gWoodenCrate.vao2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWoodenCrate.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.positionBuffer);
    glEnableVertexAttribArray(gWoodenCrate.program2->attrib("vert"));
    glVertexAttribPointer(gWoodenCrate.program2->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.normalBuffer);
    glEnableVertexAttribArray(gWoodenCrate.program->attrib("vertNormal"));
    glVertexAttribPointer(gWoodenCrate.program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // create and bind VAO for instancing
    instancingProgram = LoadShadersI();

    glGenVertexArrays(1, &instancingVao);
    glBindVertexArray(instancingVao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWoodenCrate.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.positionBuffer);
    glEnableVertexAttribArray(instancingProgram->attrib("vert"));
    glVertexAttribPointer(instancingProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.normalBuffer);
    glEnableVertexAttribArray(instancingProgram->attrib("vertNormal"));
    glVertexAttribPointer(instancingProgram->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.uvBuffer);
    glEnableVertexAttribArray(instancingProgram->attrib("vertTexCoord"));
    glVertexAttribPointer(instancingProgram->attrib("vertTexCoord"), 3, GL_FLOAT, GL_FALSE,  0,0);

    glGenBuffers(1, &instancingVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instancingVbo);
    GLuint pos = instancingProgram->attrib("model");
    for (unsigned int i = 0; i < 4 ; i++) {
        glEnableVertexAttribArray(pos + i);
        glVertexAttribPointer(pos + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
        glVertexAttribDivisor(pos + i, 1);
    }

    glGenBuffers(1, &instancingColorVbo);
    glBindBuffer(GL_ARRAY_BUFFER, instancingColorVbo);
    glEnableVertexAttribArray(instancingProgram->attrib("materialColor"));
    glVertexAttribPointer(instancingProgram->attrib("materialColor"), 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)sizeof(vec3));
    glVertexAttribDivisor(instancingProgram->attrib("materialColor"), 1);

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // create and bind VAO for drawing instancing countours
    instancingProgram2 = LoadShaders2I();

    glGenVertexArrays(1, &instancingVao2);
    glBindVertexArray(instancingVao2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWoodenCrate.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.positionBuffer);
    glEnableVertexAttribArray(instancingProgram2->attrib("vert"));
    glVertexAttribPointer(instancingProgram2->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.normalBuffer);
    glEnableVertexAttribArray(instancingProgram2->attrib("vertNormal"));
    glVertexAttribPointer(instancingProgram2->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);

    glBindBuffer(GL_ARRAY_BUFFER, instancingVbo);
    pos = instancingProgram2->attrib("model");
    for (unsigned int i = 0; i < 4 ; i++) {
        glEnableVertexAttribArray(pos + i);
        glVertexAttribPointer(pos + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
        glVertexAttribDivisor(pos + i, 1);
    }

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void LoadWorldAsset()
{
    gWorld.program = LoadShaders();
    gWorld.program2 = LoadShaders2();
    gWorld.drawType = GL_TRIANGLES;
    gWorld.drawStart = 0;
    gWorld.drawCount = meshes[1]->numIndices;

    // create and bind VAO
    glGenVertexArrays(1, &gWorld.vao);
    glBindVertexArray(gWorld.vao);

    glGenBuffers(1, &gWorld.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWorld.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[1]->numIndices * sizeof(unsigned int), meshes[1]->indices, GL_STATIC_DRAW);

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

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // create and bind VAO
    glGenVertexArrays(1, &gWorld.vao2);
    glBindVertexArray(gWorld.vao2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWorld.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, gWorld.positionBuffer);
    glEnableVertexAttribArray(gWorld.program2->attrib("vert"));
    glVertexAttribPointer(gWorld.program2->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, gWorld.normalBuffer);
    glEnableVertexAttribArray(gWorld.program->attrib("vertNormal"));
    glVertexAttribPointer(gWorld.program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
