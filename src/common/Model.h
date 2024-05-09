#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>


#include "Mesh.h"
#include "Texture2D.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>

#include <ostream>


using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
    /*  函数   */
    Model(char *path)
    {
        loadModel(path);
    }
    void draw(Shader shader, unsigned int count = 1);

    /*  模型数据  */
    vector<Mesh> meshes;
private:
    /*  模型数据  */
    vector<Texture> textures_loaded;
    string directory;
    /*  函数   */
    void loadModel(const string& path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};

void Model::draw(Shader shader, unsigned int count){
    for(int i = 0; i < meshes.size(); i++){
        meshes[i].draw(shader, count);
    }
};

void Model::loadModel(const string& path){
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }

    directory = path.substr(0, path.find_last_of("/"));
    processNode(scene->mRootNode, scene);
};

void Model::processNode(aiNode *node, const aiScene *scene){
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for(unsigned int i = 0; i < node->mNumChildren; i++){
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene){
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    
    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
        Vertex vertex;
        //position
        glm::vec3 vector;
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        vertex.position = vector;

        //normals
        if(mesh->HasNormals()){
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }

        //texcoords
        if(mesh->mTextureCoords[0]){
            glm::vec2 vec;

            vec.x = mesh->mTextureCoords[0][i].x; 
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texcoords = vec;
        }else{
            vertex.texcoords = glm::vec2(0.0f, 0.0f);
        }

        if(mesh->HasTangentsAndBitangents()){
            //tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;

            //bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        }else{
            vertex.tangent = glm::vec3(0.0f);
            vertex.bitangent = glm::vec3(0.0f);
        }
        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++){
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    
    vector<Texture> reflectMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflect");
    textures.insert(textures.end(), reflectMaps.begin(), reflectMaps.end());
    
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        
    return Mesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName){
    vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for(unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecessary load duplicate textures.
        }
    }
    return textures;
}

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma) {
    string filename = string(path);
    filename = directory + '/' + filename;

    Texture2DParameters params(
        GL_REPEAT,
        GL_REPEAT,
        GL_LINEAR_MIPMAP_LINEAR,
        GL_LINEAR
    );
    Texture2D texture2d(filename.c_str(), params);
    return texture2d.ID;
}
#endif