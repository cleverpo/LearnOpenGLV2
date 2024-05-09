#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"


struct PointLightParameters{
    glm::vec3 position;
    glm::vec3 color;
    float attenuationFactor1;   //常数项衰减系数
    float attenuationFactor2;   //一次项衰减系数
    float attenuationFactor3;   //二次项衰减系数
};

class PointLight
{
public:
    glm::vec3 position;
    glm::vec3 color;
    float attenuationFactor1;   //常数项衰减系数
    float attenuationFactor2;   //一次项衰减系数
    float attenuationFactor3;   //二次项衰减系数

    glm::mat4 matrix;
    Shader shader;

    PointLight(const PointLightParameters& params):
    position(params.position),
    color(params.color),
    attenuationFactor1(params.attenuationFactor1),
    attenuationFactor2(params.attenuationFactor2),
    attenuationFactor3(params.attenuationFactor3),
    shader(Shader("../src/common/shader/light_vert.vs", "../src/common/shader/light_frag.fs"))
    {
        this->init();  
        this->updateMatrix();
    };

    void setPosition(const glm::vec3& pos){
        this->position = pos;
        this->updateMatrix();
    }

    //渲染光源
    void render(const Camera& camera){
        shader.use();
        shader.setMat4("model", glm::value_ptr(this->matrix));
        shader.setMat4("view", glm::value_ptr(camera.matView));
        shader.setMat4("projection", glm::value_ptr(camera.matProjection));
        shader.setVec3("lightColor", glm::value_ptr(color));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    //提交数据
    void apply(Shader& shader, unsigned int index){
        glm::vec3 ambient = color * glm::vec3(0.2f);
        glm::vec3 diffuse = color * glm::vec3(0.5f);
        glm::vec3 specular = color * glm::vec3(1.0f);

        std::string prefix = "pointLights[";
        prefix += std::to_string(index);
        prefix += "]";
        
        shader.setVec3(prefix + ".position", glm::value_ptr(position));
        shader.setFloat(prefix + ".attenuationFactor1", attenuationFactor1);
        shader.setFloat(prefix + ".attenuationFactor2", attenuationFactor2);
        shader.setFloat(prefix + ".attenuationFactor3", attenuationFactor3);
        shader.setVec3(prefix + ".ambient", glm::value_ptr(ambient));
        shader.setVec3(prefix + ".diffuse", glm::value_ptr(diffuse));
        shader.setVec3(prefix + ".specular", glm::value_ptr(specular));
    }

private:
    unsigned int VAO;
    unsigned int VBO;

    void init(){
        float VERTICES[] = {
            //顶点坐标          
            -0.5f, -0.5f, -0.5f,
            0.5f,  0.5f, -0.5f, 
            0.5f, -0.5f, -0.5f, 
            0.5f,  0.5f, -0.5f, 
            -0.5f, -0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
            0.5f, -0.5f,  0.5f, 
            0.5f,  0.5f,  0.5f, 
            0.5f,  0.5f,  0.5f, 
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

            0.5f,  0.5f,  0.5f, 
            0.5f, -0.5f, -0.5f, 
            0.5f,  0.5f, -0.5f, 
            0.5f, -0.5f, -0.5f, 
            0.5f,  0.5f,  0.5f, 
            0.5f, -0.5f,  0.5f, 

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f, 
            0.5f, -0.5f,  0.5f, 
            0.5f, -0.5f,  0.5f, 
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f,  0.5f, 
            0.5f,  0.5f, -0.5f, 
            0.5f,  0.5f,  0.5f, 
            -0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f,  0.5f,
        };

        //VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

        //VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);
    };

    void updateMatrix(){
        matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::scale(matrix, glm::vec3(0.1f));
    };
};

#endif