#pragma once

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <map>

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
#include "Scene/Teapot.h"

#include "Scene/Mesh.h"

#include "Scene/Asset.cpp"
#include "Scene/Particle.cpp"

#include "cfgReader.h"

// configs
map<string,string> cfg = initCfg();

const vec2 SCREEN_SIZE(atoi(cfg.find("screenWidth")->second.c_str()), atoi(cfg.find("screenHeight")->second.c_str()));
const vec2 CENTER = SCREEN_SIZE * 0.5f;
static const bool FULLSCREEN = atoi(cfg.find("fullscreen")->second.c_str()) == 0 ? false : true;

static const int SHADOWMAP_SIZE = atoi(cfg.find("shadowMapSize")->second.c_str());

static const int NumEnemies = atoi(cfg.find("numEnemies")->second.c_str());
static const int KillsToWin = atoi(cfg.find("killsToWin")->second.c_str());

static const bool ShowShadowMap = atoi(cfg.find("showShadowMap")->second.c_str()) == 0 ? false : true;

static const bool DrawParticles = atoi(cfg.find("drawParticles")->second.c_str()) == 0 ? false : true;
static const int ParticleCount = atoi(cfg.find("particleCount")->second.c_str());
static const int TimeToLive = atoi(cfg.find("particleTTL")->second.c_str());

static const int UseCelShade = atoi(cfg.find("useCelShade")->second.c_str());
static const bool DrawContours = atoi(cfg.find("drawContours")->second.c_str()) == 0 ? false : true;

static const bool BlockyDoomMode = atoi(cfg.find("blockyDoomMode")->second.c_str()) == 0 ? false : true;
static const bool GodMode = atoi(cfg.find("godMode")->second.c_str()) == 0 ? false : true;

// global variables
GLFWwindow* window;

std::list<ModelInstance*> instances;
std::list<Particle*> particles;

ModelAsset gWoodenCrate;
ModelAsset gWorld;
ModelAsset gTeapot;

Camera* camera;
Light light;
Player* player;
World* world;
Teapot* cow;

Texture2* targetTexture;

int currentEnemies = 0;
int killCounter = 0;
bool won = false;
bool lost = false;
double timeStamp = 0;

// model loading
Assimp::Importer* importer;
vector<Mesh*> meshes;

// collision detection
btCollisionWorld* collisionWorld;

// instancing stuff
Program* instancingProgram;
Program* instancingProgramContour;

GLuint instancingVao;
GLuint instancingVaoContour;

GLuint instancingVbo;
GLuint instancingColorVbo;

// functions
GLFWwindow* openWindow(int width, int height);

// update functions
void update(double time, double deltaT);
void rotateCamera(float time, float deltaT);
void doCollisionThingy(float time, float deltaT);
void moveSpotLight(float time, float deltaT);
void respawnEnemies(float time, float deltaT);
void checkUserInput(float time, float deltaT);
void updateParticles(float time, float deltaT);
void updateInstances(float time, float deltaT);
void checkGameOver(float time, float deltaT);

// draw functions
void draw();
void drawInstance(const ModelInstance& inst);
void drawContour(const ModelInstance& inst);
void drawInstanceDepth(const ModelInstance& inst);
void drawParticles(mat4* data, vec3* colors, int size);
void drawParticlesContour(mat4* data, vec3* colors, int size);

void cleanup();

// static functions
static Program* loadShaders();
static Program* loadShadersContour();
static Program* loadDepthShaders();
static Program* loadDebugShadowShaders();
static Program* loadInstancingShaders();
static Program* loadInstancingShadersContour();

static Texture2* loadTargetTexture();
static Material* loadMaterial(vec3 color, string texPath);

static void loadWoodenCrateAsset();
static void loadWorldAsset();
static void loadTeapotAsset();

static void createWorldInstance();
static void createTeapotInstance();

static void importScene(const std::string& pFile);

// type conversion function
static btTriangleMesh* pAIMesh2btTriangleMesh(const struct aiMesh *pAIMesh);

// asset location function
static const std::string Assets(const std::string& path) {
    return "Assets/" + path;
}

// shadow mapping handles
GLuint depthFrameBuffer;
GLuint depthTexture;
GLuint depthMatrix;
GLuint shadowMappingVao;

// shadow mapping debug output handles
GLuint quadVertexbuffer;
GLuint debugShadowVao;
Program* debugShadowProgram;

int main() 
{
    // (1) init everything you need
    window = openWindow(SCREEN_SIZE.x, SCREEN_SIZE.y);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //glEnable (GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    bool running = true;
    double lastTime = 0;

    //import cube from file
    importScene(Assets("Models/cube.model")); 

    // initialise the gWoodenCrate asset
    loadWoodenCrateAsset();

    targetTexture = loadTargetTexture();

    // initialise bullet
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    // SimpleBroadphase is a brute force alternative, performing N^2 aabb overlap tests
    btDbvtBroadphase*	broadphase = new btDbvtBroadphase();

    collisionWorld = new btCollisionWorld(dispatcher, broadphase, collisionConfiguration);

    // initialise world
    importScene(Assets("Models/world2.obj"));
    loadWorldAsset();
    createWorldInstance();

    //initialise teapot
    importScene(Assets("Models/teapot.obj"));
    loadTeapotAsset();
    createTeapotInstance();

    player = new Player(&gWoodenCrate, loadMaterial(vec3(132,213,219),"Texture/noise.png"), &instances, collisionWorld);

    camera = new Camera(player);

    // generate view frustum
    // TODO

    double time = glfwGetTime();

    //camera->setPosition(vec3(0,0,4));
    camera->setViewportAspectRatio(SCREEN_SIZE.x / SCREEN_SIZE.y);

    light.position = vec3(0,0,0);
    //light.position.y = light.position.y + 15.0f;
    light.intensities = vec3(1, 1, 1) * 0.9f;
    light.attenuation = 0.0005f;
    light.ambientCoefficient = BlockyDoomMode ? 0.f : 0.50f;

    //light.direction=normalize(player->position()-camera->position());
    light.direction = vec3(0,-1,0);
    light.range = 100;

    glGenVertexArrays(1, &shadowMappingVao);
    glBindVertexArray(shadowMappingVao);

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    depthFrameBuffer = 0;
    glGenFramebuffers(1, &depthFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);

    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT, SHADOWMAP_SIZE, SHADOWMAP_SIZE, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(vec4(1, 1, 1, 1)));

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindVertexArray(0);


    glGenVertexArrays(1, &debugShadowVao);
    glBindVertexArray(debugShadowVao);

    if (ShowShadowMap) debugShadowProgram = loadDebugShadowShaders();

    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
    };

    glGenBuffers(1, &quadVertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    while (running && !glfwWindowShouldClose(window))
    {
        // (3) compute the frame time delta
        time = glfwGetTime();
        double deltaT = time - lastTime;
        lastTime = time;

        // (4) react to user input
        glfwPollEvents();
        running = !glfwGetKey(window, GLFW_KEY_ESCAPE);

        // (5) Update all game components
        update(time, deltaT);

        // (6) draw all game components
        draw();
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

void update(double timed, double deltaTd) 
{
    float deltaT = float(deltaTd);
    float time = float(timed);

    doCollisionThingy(time, deltaT);
    checkUserInput(time, deltaT);
    updateInstances(time, deltaT);

    updateParticles(time, deltaT);

    rotateCamera(time, deltaT);
    moveSpotLight(time, deltaT);
    respawnEnemies(time, deltaT);
    checkGameOver(time, deltaT);
}

void doCollisionThingy(float time, float deltaT) {
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
}

void moveSpotLight(float time, float deltaT) {
    if (!lost) {
        //move light
        light.position.z = light.position.z + 5;
        light.direction = normalize(vec3(player->position().x-camera->position().x,0,player->position().z-camera->position().z));
        light.position = player->position() + light.direction * 2.f;
        light.position.y += 3.f;
    } else {
        light.range = 0;
    }
}

void respawnEnemies(float time, float deltaT) {
    //fireworks
    if(won){
        if(time - timeStamp >= 0.3){

            Enemy* enemy = new Enemy(&gWoodenCrate, time, player, loadMaterial(vec3(255,153,153),"Texture/noise.png"), &instances, collisionWorld, player->position());
            timeStamp = time;
        }

    }
    else if(lost){

        if(time-timeStamp >= 0.5){
            /* light.position = player->position();
            light.position.y += 5;
            light.direction = vec3(0,-1,0);*/
        }
    }
    //spawn enemies
    else{

        for (int i = 0; i < NumEnemies - currentEnemies; i++) {
            Enemy* enemy = new Enemy(&gWoodenCrate, time, player, loadMaterial(vec3(255,153,153),"Texture/noise.png"), &instances, collisionWorld,player->position());
            currentEnemies++;
        }
    }
}

void checkUserInput(float time, float deltaT) 
{
    if (!lost) {
        if(glfwGetKey(window, 'S')){
            player->moveBackward(time, deltaT);
        } else if(glfwGetKey(window, 'W')){
            player->moveForward(time, deltaT);
        }

        if(glfwGetKey(window, 'A')){
            player->moveLeft(time, deltaT);
        } else if(glfwGetKey(window, 'D')){
            player->moveRight(time, deltaT);
        }

        if(glfwGetKey(window, GLFW_KEY_SPACE)) {
            player->jump(time, deltaT);
        }

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1)) {
            player->shoot(time, deltaT);
        }
    }
}

void checkGameOver(float time, float deltaT) {
    if(won == false & killCounter >= KillsToWin){
        won = true;
        cout << "YOU WON!!! TIME: ";
        cout << time << endl;
        timeStamp = time;
    }
}

void updateInstances(float time, float deltaT) {
    std::list<ModelInstance*>::const_iterator it;
    for(it = instances.begin(); it != instances.end();) {
        ModelInstance* instance = *it;

        instance->update(time, deltaT);

        //check if win condition is meet
        if(killCounter >= KillsToWin){
            if(dynamic_cast<Enemy*>(instance)){
                instance->markDeleted();
            }
        }

        if(instance->getHit() && lost == false && !GodMode){
            lost = true;
            timeStamp = time;

            if (DrawParticles) {
                // player death animation
                for (int i = 0; i < 5 * ParticleCount; i++) {
                    Particle* part = new Particle(instance->position(), instance->material, instance->asset, time, 40);
                    particles.push_back(part);
                }
            }
        }

        if(lost && timeStamp != 0 || won && timeStamp != 0){
            if(Enemy* e = dynamic_cast<Enemy*>(instance)){
                e->stopShooting();
            }
        }

        if (instance->isMarkedDeleted()) {
            //printf("Deleted something of %s\n", typeid(*instance).name());

            if(dynamic_cast<Enemy*>(instance)){
                currentEnemies--;
                killCounter++;       
            }

            if (DrawParticles) {
                if (dynamic_cast<Bullet*>(instance)) {
                    for (int i = 0; i < ParticleCount; i++) {
                        Particle* part = new Particle(instance->position(), instance->material, instance->asset, time, 20);
                        particles.push_back(part);
                    }
                } else if (dynamic_cast<Enemy*>(instance)) {
                    for (int i = 0; i < 5 * ParticleCount; i++) {
                        Particle* part = new Particle(instance->position(), instance->material, instance->asset, time, 40);
                        particles.push_back(part);
                    }
                }
            }

            it = instances.erase(it);
            delete instance;
        } else {
            ++it;
        }
    }
}

void updateParticles(float time, float deltaT) {
    // update "particles"
    std::list<Particle*>::const_iterator it2;
    for (it2 = particles.begin(); it2 != particles.end();) {

        Particle* part = *it2;
        part->update(time, deltaT);

        if (part->_distance > TimeToLive) {
            it2 = particles.erase(it2);
            delete part;
        } else {
            ++it2;
        }
    }
}

void rotateCamera(float time, float deltaT)
{
    //rotate camera based on mouse movement
    const float mouseSensitivity = 0.1;

    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    float diffX = mouseX - CENTER.x;
    float diffY = mouseY - CENTER.y;

    camera->offsetOrienatation(mouseSensitivity * diffY, mouseSensitivity * diffX);

    glfwSetCursorPos(window, CENTER.x, CENTER.y); //reset the mouse, so it doesn't go out of the window
}

void draw() 
{
    std::list<ModelInstance*>::const_iterator it;

    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthFrameBuffer);
    glViewport(0,0,SHADOWMAP_SIZE,SHADOWMAP_SIZE);

    glCullFace(GL_FRONT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(it = instances.begin(); it != instances.end(); ++it){
        if((*it)->getHit() && !GodMode){
            continue;
        }
        drawInstanceDepth(*(*it));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,256,256);

    glCullFace(GL_BACK);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if (ShowShadowMap) {
        glBindVertexArray(debugShadowVao);

        // Use our shader
        glUseProgram(debugShadowProgram->object());

        glBindBuffer(GL_ARRAY_BUFFER, quadVertexbuffer);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        // Set our "renderedTexture" sampler to user Texture Unit 0
        glUniform1i(debugShadowProgram->uniform("texture"), 3);

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
    }

    glViewport(0,0,SCREEN_SIZE.x,SCREEN_SIZE.y);

    // clear the frame and depth buffer
    //vec3 bg = vec3(225,209,244) / 255.0f;
    //glClearColor(bg.r, bg.g, bg.b, 1);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // cel shader
    glUseProgram(player->asset->program->object());
    glCullFace(GL_BACK);
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    for(it = instances.begin(); it != instances.end(); ++it){
        if((*it)->getHit() && !GodMode){
            continue;
        }
        drawInstance(*(*it));
    }

    if (DrawContours) {
        // contour shader
        glUseProgram(player->asset->program2->object());
        glCullFace(GL_FRONT);
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        for(it = instances.begin(); it != instances.end(); ++it){
            if((*it)->getHit() && !GodMode){
                continue;
            }
            drawContour(*(*it));
        }
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
        glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(instancingProgram->object());
        glCullFace(GL_BACK);
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        drawParticles(data, colors, size);

        if (DrawContours) {
            // contour shader
            glUseProgram(instancingProgramContour->object());
            glCullFace(GL_FRONT);
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

            drawParticlesContour(data, colors, size);
        }

        glDisable(GL_BLEND);

        delete data;
        delete colors;
    }

    // clear
    //glUseProgram(0);
}

static mat4 calcDepthMatrix(const ModelInstance& inst) {
    //glm::vec3 lightInvDir = glm::vec3(0.5,1,0);

    mat4 depthProjectionMatrix = glm::perspective<float>(90.0f, 1.0f, 2.0f, 200.f);
    mat4 depthViewMatrix = glm::lookAt(light.position, light.position + light.range * light.direction, vec3(0,1,0));

    mat4 depthModelMatrix = inst.transform;

    return depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
}

void drawInstanceDepth(const ModelInstance& inst){

    ModelAsset* asset = inst.asset;
    Program* program = asset->shadowProgram;

    glUseProgram(program->object());
    glBindVertexArray(asset->shadowVao);

    mat4 depthMVP = calcDepthMatrix(inst);

    glUniformMatrix4fv(program->uniform("depthMVP"), 1, GL_FALSE, glm::value_ptr(depthMVP));

    glDrawElements(inst.asset->drawType, inst.asset->drawCount, GL_UNSIGNED_INT, 0);

    //glBindVertexArray(0);
}

void drawParticles(mat4* data, vec3* colors, int size)
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

    glUniform1i(program->uniform("UseCelShade" ), UseCelShade);

    // set the shader uniforms
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, glm::value_ptr(camera->matrix()));
    glUniform3fv(program->uniform("cameraPosition"), 1, glm::value_ptr(camera->position()));

    // bind the texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->texture->object());
    glUniform1i(program->uniform("tex"), 0);

    glUniform3fv(program->uniform("light.position"), 1, glm::value_ptr(light.position));
    glUniform3fv(program->uniform("light.intensities"), 1, glm::value_ptr(light.intensities));
    glUniform1f(program->uniform("light.attenuation"), light.attenuation);
    glUniform1f(program->uniform("light.ambientCoefficient"), light.ambientCoefficient);

    //glUniform3fv(program->uniform("material.color"), 1, glm::value_ptr(material->color)); 
    glUniform3fv(program->uniform("material.specularColor"), 1, glm::value_ptr(material->specularColor));
    glUniform1f(program->uniform("material.shininess"), material->shininess);

    glDrawElementsInstanced(asset->drawType, asset->drawCount, GL_UNSIGNED_INT, 0, size);

    // unbind vao, vbo and texture
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawParticlesContour(mat4* data, vec3* colors, int size)
{
    glBindBuffer(GL_ARRAY_BUFFER, instancingVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * size, &data[0], GL_DYNAMIC_DRAW);

    Particle* p = particles.front();

    ModelAsset* asset = p->asset;
    //Material* material = p->material;
    Program* program = instancingProgramContour;

    // bind VAO
    glBindVertexArray(instancingVaoContour);

    // set the shader uniforms
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, glm::value_ptr(camera->matrix()));

    glDrawElementsInstanced(asset->drawType, asset->drawCount, GL_UNSIGNED_INT, 0, size);

    // unbind vao, vbo and texture
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void drawInstance(const ModelInstance& inst)
{
    ModelAsset* asset = inst.asset;
    Material* material = inst.material;
    Program* program = asset->program;

    mat4 biasMatrix = mat4(
        0.5, 0.0, 0.0, 0.0, 
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
        );

    mat4 depthMVP = calcDepthMatrix(inst);
    mat4 depthBiasMVP = biasMatrix * depthMVP;


    // projection

    vec3 shootDir = glm::rotate(vec3(0,0,1), player->_verticalAngle, vec3(1,0,0));
    shootDir = glm::normalize(glm::rotate(shootDir, player->_horizontalAngle, vec3(0,1,0)));

    mat4 projectorP = glm::perspective(3.0f, 1.f, 0.1f, 200.f);
    //mat4 projectorP = glm::ortho<float>(-1.5,1.5,-1.5,1.5, .1f, 200);
    mat4 projectorV = glm::lookAt(player->position() + shootDir, player->position() + 2.f * shootDir, vec3(0,1,0));

    mat4 mModel = inst.transform;
    mat4 texMVP = projectorP * projectorV * mModel;

    mat4 texGenMatrix = biasMatrix * texMVP;

    // bind VAO
    glBindVertexArray(asset->vao);

    glUniform1i(program->uniform("UseCelShade" ), UseCelShade);

    // set the shader uniforms
    glUniformMatrix4fv(program->uniform("camera"), 1, GL_FALSE, glm::value_ptr(camera->matrix()));
    glUniform3fv(program->uniform("cameraPosition"), 1, glm::value_ptr(camera->position()));

    glUniformMatrix4fv(program->uniform("model"), 1, GL_FALSE, glm::value_ptr(inst.transform));

    glUniformMatrix4fv(program->uniform("depthBias"), 1, GL_FALSE, &depthBiasMVP[0][0]);
    glUniformMatrix4fv(program->uniform("texGenMatrix"), 1, GL_FALSE, &texGenMatrix[0][0]);

    glUniform3fv(program->uniform("light.position"), 1, glm::value_ptr(light.position));
    glUniform3fv(program->uniform("light.intensities"), 1, glm::value_ptr(light.intensities));
    glUniform1f(program->uniform("light.attenuation"), light.attenuation);
    glUniform1f(program->uniform("light.ambientCoefficient"), light.ambientCoefficient);
    glUniform3fv(program->uniform("light.direction"), 1, glm::value_ptr(light.direction));
    glUniform1f(program->uniform("light.range"), light.range);

    glUniform3fv(program->uniform("material.color"), 1, glm::value_ptr(material->color)); 
    glUniform3fv(program->uniform("material.specularColor"), 1, glm::value_ptr(material->specularColor));
    glUniform1f(program->uniform("material.shininess"), material->shininess);

    // bind the textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material->texture->object());
    glUniform1i(program->uniform("tex"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(program->uniform("shadowMap"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, targetTexture->object());
    glUniform1i(program->uniform("targetTex"), 2);

    glDrawElements(asset->drawType, asset->drawCount, GL_UNSIGNED_INT, 0);

    // unbind vao and texture
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_2D, 1);
}

void drawContour(const ModelInstance& inst) 
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


    GLFWmonitor* fullscreen = nullptr;
    if(FULLSCREEN){
        fullscreen = glfwGetPrimaryMonitor();
    }
    // (3) open window  
    GLFWwindow* window = glfwCreateWindow(width, height, "Blocky Blocks", fullscreen, nullptr);

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

static Texture2* loadTargetTexture()
{
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(Assets("Texture/target.png"));
    return new Texture2(bmp);
}

static Program* loadShaders() 
{
    Shader* vertexShader = new Shader(Assets("Shader/shadowVertexShader.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/shadowCelShader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);

}

static Program* loadShadersContour() 
{
    Shader* vertexShader = new Shader(Assets("Shader/vertexShader.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/blackShader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Program* loadInstancingShaders() 
{
    Shader* vertexShader = new Shader(Assets("Shader/instancingShader.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/instancingCelShader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Program* loadInstancingShadersContour() 
{
    Shader* vertexShader = new Shader(Assets("Shader/instancingShaderContour.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/blackShader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Program* loadDepthShaders() 
{
    Shader* vertexShader = new Shader(Assets("Shader/depthShader.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/depthShader.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Program* loadDebugShadowShaders() 
{
    Shader* vertexShader = new Shader(Assets("Shader/debugShadow.vert").c_str(), GL_VERTEX_SHADER);
    Shader* fragmentShader = new Shader(Assets("Shader/debugShadow.frag").c_str(), GL_FRAGMENT_SHADER);
    return new Program(vertexShader, fragmentShader);
}

static Material* loadMaterial(vec3 color, string texPath)
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

static void importScene(const std::string& pFile) {

    importer = new Assimp::Importer();
    const aiScene* scene = importer->ReadFile(pFile,
        aiProcess_Triangulate               |
        aiProcess_GenSmoothNormals          |
        aiProcess_JoinIdenticalVertices     |
        aiProcess_ImproveCacheLocality      |
        aiProcess_SortByPType);

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

static void createWorldInstance()
{
    Material* m = loadMaterial(vec3(192,158,233),"Texture/noise.png");
    btTriangleMesh* triMesh = pAIMesh2btTriangleMesh(meshes[1]->mesh);
    world = new World(&gWorld, m, triMesh, &instances, collisionWorld);
}

static btTriangleMesh* pAIMesh2btTriangleMesh(const struct aiMesh *pAIMesh)
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

static void loadWoodenCrateAsset()
{
    gWoodenCrate.drawType = GL_TRIANGLES;
    gWoodenCrate.drawStart = 0;
    gWoodenCrate.drawCount = meshes[0]->numIndices;

    // create and bind VAO
    gWoodenCrate.program = loadShaders();

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
    gWoodenCrate.program2 = loadShadersContour();

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
    instancingProgram = loadInstancingShaders();

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
    instancingProgramContour = loadInstancingShadersContour();

    glGenVertexArrays(1, &instancingVaoContour);
    glBindVertexArray(instancingVaoContour);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWoodenCrate.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.positionBuffer);
    glEnableVertexAttribArray(instancingProgramContour->attrib("vert"));
    glVertexAttribPointer(instancingProgramContour->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.normalBuffer);
    glEnableVertexAttribArray(instancingProgramContour->attrib("vertNormal"));
    glVertexAttribPointer(instancingProgramContour->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);

    glBindBuffer(GL_ARRAY_BUFFER, instancingVbo);
    pos = instancingProgramContour->attrib("model");
    for (unsigned int i = 0; i < 4 ; i++) {
        glEnableVertexAttribArray(pos + i);
        glVertexAttribPointer(pos + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
        glVertexAttribDivisor(pos + i, 1);
    }

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // create and bind VAO for drawing instancing countours
    gWoodenCrate.shadowProgram = loadDepthShaders();

    glGenVertexArrays(1, &gWoodenCrate.shadowVao);
    glBindVertexArray(gWoodenCrate.shadowVao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWoodenCrate.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.positionBuffer);
    glEnableVertexAttribArray(gWoodenCrate.shadowProgram->attrib("vert"));
    glVertexAttribPointer(gWoodenCrate.shadowProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    /*
    glBindBuffer(GL_ARRAY_BUFFER, gWoodenCrate.normalBuffer);
    glEnableVertexAttribArray(shadowProgramCube->attrib("vertNormal"));
    glVertexAttribPointer(shadowProgramCube->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);
    */

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void loadWorldAsset()
{
    gWorld.program = loadShaders();
    gWorld.program2 = loadShadersContour();
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

    // create and bind VAO for drawing instancing countours
    gWorld.shadowProgram = loadDepthShaders();

    glGenVertexArrays(1, &gWorld.shadowVao);
    glBindVertexArray(gWorld.shadowVao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gWorld.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, gWorld.positionBuffer);
    glEnableVertexAttribArray(gWorld.shadowProgram->attrib("vert"));
    glVertexAttribPointer(gWorld.shadowProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    /*
    glBindBuffer(GL_ARRAY_BUFFER, gWorld.normalBuffer);
    glEnableVertexAttribArray(shadowProgramWorld->attrib("vertNormal"));
    glVertexAttribPointer(shadowProgramWorld->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);
    */

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void loadTeapotAsset()
{
    gTeapot.program = loadShaders();
    gTeapot.program2 = loadShadersContour();
    gTeapot.drawType = GL_TRIANGLES;
    gTeapot.drawStart = 0;
    gTeapot.drawCount = meshes[2]->numIndices;

    // create and bind VAO
    glGenVertexArrays(1, &gTeapot.vao);
    glBindVertexArray(gTeapot.vao);

    glGenBuffers(1, &gTeapot.indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gTeapot.indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[2]->numIndices * sizeof(unsigned int), meshes[2]->indices, GL_STATIC_DRAW);

    glGenBuffers(1, &gTeapot.positionBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gTeapot.positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, meshes[2]->numVertices* 3 * sizeof(float), meshes[2]->vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gTeapot.program->attrib("vert"));
    glVertexAttribPointer(gTeapot.program->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &gTeapot.normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gTeapot.normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, meshes[2]->numVertices* 3 * sizeof(float), meshes[2]->normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gTeapot.program->attrib("vertNormal"));
    glVertexAttribPointer(gTeapot.program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);

    glGenBuffers(1, &gTeapot.uvBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, gTeapot.uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, meshes[2]->numVertices* 3 * sizeof(float), meshes[2]->textureCoords, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gTeapot.program->attrib("vertTexCoord"));
    glVertexAttribPointer(gTeapot.program->attrib("vertTexCoord"), 2, GL_FLOAT, GL_FALSE,  3*sizeof(float), 0);

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // create and bind VAO
    glGenVertexArrays(1, &gTeapot.vao2);
    glBindVertexArray(gTeapot.vao2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gTeapot.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, gTeapot.positionBuffer);
    glEnableVertexAttribArray(gTeapot.program2->attrib("vert"));
    glVertexAttribPointer(gTeapot.program2->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, gTeapot.normalBuffer);
    glEnableVertexAttribArray(gTeapot.program->attrib("vertNormal"));
    glVertexAttribPointer(gTeapot.program->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // create and bind VAO for drawing instancing countours
    gTeapot.shadowProgram = loadDepthShaders();

    glGenVertexArrays(1, &gTeapot.shadowVao);
    glBindVertexArray(gTeapot.shadowVao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gTeapot.indexBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, gTeapot.positionBuffer);
    glEnableVertexAttribArray(gTeapot.shadowProgram->attrib("vert"));
    glVertexAttribPointer(gTeapot.shadowProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 0, 0);

    /*
    glBindBuffer(GL_ARRAY_BUFFER, gTeapot.normalBuffer);
    glEnableVertexAttribArray(shadowProgramWorld->attrib("vertNormal"));
    glVertexAttribPointer(shadowProgramWorld->attrib("vertNormal"), 3, GL_FLOAT, GL_FALSE,  0,0);
    */

    // unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void createTeapotInstance()
{
    Material* m = loadMaterial(vec3(255,255,255),"Texture/noise.png");
    btTriangleMesh* triMesh = pAIMesh2btTriangleMesh(meshes[2]->mesh);
    cow = new Teapot(&gTeapot, m, triMesh, &instances, collisionWorld);
}
