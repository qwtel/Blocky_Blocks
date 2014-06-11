#pragma once

#include <assimp/Importer.hpp>      
#include <assimp/scene.h>           
#include <assimp/postprocess.h>
#include <GL/glew.h>

class Mesh
{
public:
    Mesh(aiMesh* mesh, unsigned int numVertices, aiVector3D* vertices,
        aiVector3D* normals, unsigned int numFaces, aiFace* faces, aiVector3D* textureCoords);
    ~Mesh(void);

    aiVector3D* vertices;
    aiVector3D* normals;
    aiVector3D* textureCoords;
    GLuint* indices;

    GLuint numVertices;
    GLuint numIndices;

    aiMesh* mesh;
};

