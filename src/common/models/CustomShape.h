#ifndef CUSTOM_SHAPE_H
#define CUSTOM_SHAPE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "BaseShape.h"

#include <ostream>

using namespace std;


class CustomShape : public BaseShape
{
public:
    /*  函数   */
    CustomShape(vector<Vertex> vertices, vector<unsigned int> indices, const char* texturePath)
    {
        loadModel(vertices, indices, texturePath);
    }

private:
    /*  函数   */
    void loadModel(vector<Vertex> vertices, vector<unsigned int> indices, const char* texturePath);
};

void CustomShape::loadModel(vector<Vertex> vertices, vector<unsigned int> indices, const char* texturePath){
    vector<Texture> textures;

    if(texturePath){
        Texture texture;
        texture.id = textureFromFile(texturePath);
        texture.type = "texture_diffuse";
        texture.path = string(texturePath);
        textures.push_back(texture);
    }

    meshes.push_back(Mesh(vertices, indices, textures));
};
#endif