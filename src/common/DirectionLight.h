#ifndef DIRECTION_LIGHT_H
#define DIRECTION_LIGHT_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"
#include "GLStateUtils.h"

struct DirectionLightParameters{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
};

class DirectionLight
{
public:
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 color;

    glm::mat4 matrix;
    Shader shader;

    DirectionLight(const DirectionLightParameters& params):
    position(params.position),
    direction(params.direction),
    color(params.color),
    shader(Shader("../src/common/shader/light_vert.vs", "../src/common/shader/light_frag.fs"))
    {
        this->init();  
        this->updateMatrix();
    };

    void setPosition(const glm::vec3& pos){
        this->position = pos;
        this->updateMatrix();
    }

    void apply(Shader& shader){
        glm::vec3 ambient = color * glm::vec3(0.2f);
        glm::vec3 diffuse = color * glm::vec3(1.0f);
        glm::vec3 specular = color * glm::vec3(1.0f);
        glm::vec3 dir = -position;
        shader.setVec3("directionLight.direction", glm::value_ptr(dir));
        shader.setVec3("directionLight.ambient", glm::value_ptr(ambient));
        shader.setVec3("directionLight.diffuse", glm::value_ptr(diffuse));
        shader.setVec3("directionLight.specular", glm::value_ptr(specular));
    }

    //光照空间的变换矩阵
    void applyLightSpaceMatrix(Shader& shader){
        glm::mat4 lightProjectionMat, lightViewMat;
        glm::mat4 lightSpaceMat;

        float near_plane = 1.0f, far_plane = 7.5f;
        lightProjectionMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        lightViewMat = glm::lookAt(position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        // lightViewMat = glm::lookAt(glm::vec3(0.0f), glm::normalize(direction), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMat = lightProjectionMat * lightViewMat;

        shader.setMat4("lightSpaceMatrix", glm::value_ptr(lightSpaceMat));
    }

    //渲染光源
    void render(const Camera& camera){
        shader.use();
        shader.setMat4("model", glm::value_ptr(this->matrix));
        shader.setMat4("view", glm::value_ptr(camera.matView));
        shader.setMat4("projection", glm::value_ptr(camera.matProjection));
        shader.setVec3("lightColor", glm::value_ptr(color));

        // bool isCullFace = GLStateUtils::getInstance()->isCullFace();
        // GLenum callFaceMode = GLStateUtils::getInstance()->getCullFaceMode();
        
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // if(isCullFace){
        //     glEnable(GL_CULL_FACE);
        //     glCullFace(callFaceMode);
        // }else{
        //     glDisable(GL_CULL_FACE);
        // }
    }

private:
    unsigned int VAO;
    unsigned int VBO;

    void init(){
        float VERTICES[] = {
            -0.5f, -0.5f, 0.0f,   //左下角
            0.5f, -0.5f, 0.0f,    //右下角
            0.5f, 0.5f, 0.0f,     //右上角 

            -0.5f, 0.5, 0.0f,    //左上角
            -0.5f, -0.5f, 0.0f,   //左下角
            0.5f, 0.5f, 0.0f,     //右上角 
            // -0.5f, 0.0f, 0.5f,    //左下角
            // 0.5f, 0.0f, 0.5f,    //右下角
            // 0.5f, 0.0f, -0.5f,     //右上角 

            // -0.5f, 0.0f, -0.5,    //左上角
            // -0.5f, 0.0f, 0.5f,   //左下角
            // 0.5f, 0.0f, -0.5f,     //右上角 
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
        glm::vec3 zAxis = glm::vec3(0.00001f, 0.00001f, 1.0f);
        glm::vec3 dir = glm::normalize(-position);
        glm::vec3 axis = glm::cross(zAxis, dir);
        float radians = glm::acos(glm::dot(dir, zAxis));
        // matrix = glm::lookAt(position, position + glm::normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, radians, axis);
        matrix = glm::scale(matrix, glm::vec3(0.5f));
    };
};

#endif