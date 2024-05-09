#ifndef SHAPE_H
#define SHAPE_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "BaseShape.h"

#include <ostream>

using namespace std;

//顶点数据
//立方体
float CUBE_VERTICES[] = {
    //顶点坐标              //法向量                //UV
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,    0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
};

//矩形
float QUAD_VERTICES[] = {
    0.5f, 0.5f, 0.0f,       1.0f, 1.0f, 1.0f,       1.0f, 1.0f, //右上角 
    0.5f, -0.5f, 0.0f,      1.0f, 1.0f, 1.0f,       1.0f, 0.0f, //右下角
    -0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, //左下角

    -0.5f, 0.5f, 0.0f,      1.0f, 1.0f, 1.0f,       0.0f, 1.0f, //左上角
    0.5f, 0.5f, 0.0f,       1.0f, 1.0f, 1.0f,       1.0f, 1.0f, //右上角 
    -0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,       0.0f, 0.0f, //左下角
};

float PLANE_VERTICES[] = {
    25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
    -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,

    25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
    25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f,
    -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
};

enum SHAPE_TYPE{
    CUBE,
    QUAD,
    PLANE
};

class Shape : public BaseShape
{
public:
    /*  函数   */
    Shape(SHAPE_TYPE type, const char* texturePath)
    :BaseShape()
    {
        loadModel(type, texturePath);
    }

private:
    /*  函数   */
    void loadModel(SHAPE_TYPE type, const char* texturePath);
};

void Shape::loadModel(SHAPE_TYPE type, const char* texturePath){
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    float* vertices_array;
    unsigned int vertices_count;
    switch(type){
        case CUBE:
            {
                vertices_array = &CUBE_VERTICES[0];
                vertices_count = 36;
            }
            break;

        case QUAD:
            {
                vertices_array = &QUAD_VERTICES[0];
                vertices_count = 6;
            }
            break;

        case PLANE:
            {
                vertices_array = &PLANE_VERTICES[0];
                vertices_count = 6;
            }
            break;
    }
    for(unsigned int i = 0; i < vertices_count; i++){
        Vertex vertex;
        //position
        glm::vec3 vector;
        vector.x = vertices_array[i * 8];
        vector.y = vertices_array[i * 8 + 1];
        vector.z = vertices_array[i * 8 + 2];

        vertex.position = vector;

        //normals
        vector.x = vertices_array[i * 8 + 3];
        vector.y = vertices_array[i * 8 + 4];
        vector.z = vertices_array[i * 8 + 5];
        vertex.normal = vector;

        //texcoords
        glm::vec2 vec;
        vec.x = vertices_array[i * 8 + 6];
        vec.y = vertices_array[i * 8 + 7];
        vertex.texcoords = vec;

        //tangent,bitangent
        vertex.tangent = glm::vec3(0.0f);
        vertex.bitangent = glm::vec3(0.0f);
        vertices.push_back(vertex);
    }

    for(unsigned int i = 0; i < vertices_count * 8; i++){
        indices.push_back(i);
    }

    if(texturePath){
        Texture texture1;
        texture1.id = textureFromFile(texturePath);
        texture1.type = "texture_diffuse";
        texture1.path = string(texturePath);
        textures.push_back(texture1);
        
        Texture texture2;
        texture2.id = textureFromFile(texturePath);
        texture2.type = "texture_specular";
        texture2.path = string(texturePath);
        textures.push_back(texture2);
    }

    meshes.push_back(Mesh(vertices, indices, textures));
};
#endif