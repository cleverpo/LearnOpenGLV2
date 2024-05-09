#ifndef BASE_SHAPE_H
#define BASE_SHAPE_H

#include <glad/glad.h>
#include <glm/glm.hpp>


#include "../Mesh.h"
#include "../Texture2D.h"

#include <ostream>

using namespace std;

class BaseShape
{
public:
    /*  函数   */
    BaseShape()
    {
    }
    void draw(Shader shader, unsigned int count = 1);

    void setTextureWrapMode(const int& wrapS, const int& wrapT);

    void updateTexture(const string& type, unsigned int textureId);

protected:
    /*  模型数据  */
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;

    unsigned int textureFromFile(const char *path);
};

void BaseShape::draw(Shader shader, unsigned int count){
    for(int i = 0; i < meshes.size(); i++){
        meshes[i].draw(shader, count);
    }
};

void BaseShape::setTextureWrapMode(const int& wrapS, const int& wrapT){
    for(unsigned int i = 0; i < meshes.size(); i++){
        vector<Texture> textures = meshes[i].textures;
        for(unsigned int j = 0; j < textures.size(); j++){
            glBindTexture(GL_TEXTURE_2D, textures[j].id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
};

void BaseShape::updateTexture(const string& type, unsigned int textureId){
    for(unsigned int i = 0; i < meshes.size(); i++){
        vector<Texture>& textures = meshes[i].textures;
        bool isFound = false;
        for(unsigned int j = 0; j < textures.size(); j++){
            if(type == textures[i].type){
                textures[i].id = textureId;
                isFound = true;
            }
        }

        if(!isFound){
            Texture tex;
            tex.id = textureId;
            tex.path = "";
            tex.type = type;
            textures.push_back(tex);
        }
    }
}

unsigned int BaseShape::textureFromFile(const char *path) {
    Texture2DParameters params(
        GL_REPEAT,
        GL_REPEAT,
        GL_LINEAR_MIPMAP_LINEAR,
        GL_LINEAR
    );
    Texture2D texture2d(path, params);
    return texture2d.ID;
}
#endif