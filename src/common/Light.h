#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"

class Light
{
public:
    glm::vec3 position;

    glm::vec3 color;

    glm::mat4 matrix;
    Shader shader;

    Light(const glm::vec3& pos, const glm::vec3& color):
    position(pos),
    color(color),
    shader(Shader("../src/common/shader/light_vert.vs", "../src/common/shader/light_frag.fs"))
    {
        this->init();  
        this->updateMatrix();
    };

    void setPosition(const glm::vec3& pos){
        this->position = pos;
        this->updateMatrix();
    }

    //获取环境光，漫反射光，镜面反射光颜色
    void getLightColor(glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular){
        ambient = color * glm::vec3(0.2f);
        diffuse = color * glm::vec3(0.5f);
        specular = color * glm::vec3(1.0f);
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

private:
    unsigned int VAO;
    unsigned int VBO;

    void init(){
        float VERTICES[] = {
            //顶点坐标          
            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f, 
            0.5f,  0.5f, -0.5f, 
            0.5f,  0.5f, -0.5f, 
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

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
            0.5f,  0.5f, -0.5f, 
            0.5f, -0.5f, -0.5f, 
            0.5f, -0.5f, -0.5f, 
            0.5f, -0.5f,  0.5f, 
            0.5f,  0.5f,  0.5f, 

            -0.5f, -0.5f, -0.5f,
            0.5f, -0.5f, -0.5f, 
            0.5f, -0.5f,  0.5f, 
            0.5f, -0.5f,  0.5f, 
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
            0.5f,  0.5f, -0.5f, 
            0.5f,  0.5f,  0.5f, 
            0.5f,  0.5f,  0.5f, 
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
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
        matrix = glm::scale(matrix, glm::vec3(0.2f));
    };
};

#endif