#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

using namespace std;

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh{
public:
    /* 网格数据 */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    unsigned int VAO;

    /* 函数 */
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void draw(Shader shader, unsigned int count);

    void updateTextures(const vector<Texture>& textures);

private:
    /* 渲染数据 */
    unsigned int VBO, EBO;
    /* 函数 */
    void setupMesh();
};

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures):
vertices(vertices),
indices(indices),
textures(textures)
{
    this->setupMesh();
};

void Mesh::updateTextures(const vector<Texture>& textures){
    this->textures = textures;
};

void Mesh::draw(Shader shader, unsigned int count){
    unsigned int diffuseNum = 1;
    unsigned int specularNum = 1;
    unsigned int normalNum = 1;
    unsigned int heightNum = 1;
    unsigned int reflectNum = 1;

    for(int i = 0; i < textures.size(); i++){
        Texture* texture = &textures[i];

        string number;
        string type = texture->type;

        if(type == "texture_diffuse")
            number = std::to_string(diffuseNum++);
        else if(type == "texture_specular")
            number = std::to_string(specularNum++); // transfer unsigned int to string
        else if(type == "texture_normal")
            number = std::to_string(normalNum++); // transfer unsigned int to string
        else if(type == "texture_height")
            number = std::to_string(heightNum++); // transfer unsigned int to string
        else if(type == "texture_reflect")
            number = std::to_string(reflectNum++); // transfer unsigned int to string

        glActiveTexture(GL_TEXTURE0 + i);
        shader.setInt(type + number, i);
        glBindTexture(GL_TEXTURE_2D, texture->id);
    }
    
    glBindVertexArray(VAO);
    if(count == 1){
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    }else{
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, count);
    }
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
};

void Mesh::setupMesh(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    //position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    //normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    
    //texcoods
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoords));

    //tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));

    //bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

    glBindVertexArray(0);
};

#endif