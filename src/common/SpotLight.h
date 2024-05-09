#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Camera.h"


struct SpotLightParameters{
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float cutoff;       //圆锥内角，这里是用cos值
    float outerCutoff;  //圆锥外角，用于边缘平滑过渡
    float attenuationFactor1;   //常数项衰减系数
    float attenuationFactor2;   //一次项衰减系数
    float attenuationFactor3;   //二次项衰减系数
};

class SpotLight
{
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 color;
    float cutoff;       //圆锥内角，这里是用cos值
    float outerCutoff;  //圆锥外角，用于边缘平滑过渡
    float attenuationFactor1;   //常数项衰减系数
    float attenuationFactor2;   //一次项衰减系数
    float attenuationFactor3;   //二次项衰减系数

    glm::mat4 matrix;
    Shader shader;

    SpotLight(const SpotLightParameters& params):
    position(params.position),
    direction(params.direction),
    color(params.color),
    cutoff(params.cutoff),
    outerCutoff(params.outerCutoff),
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

    //提交数据
    void apply(Shader& shader){
        glm::vec3 ambient = color * glm::vec3(0.2f);
        glm::vec3 diffuse = color * glm::vec3(0.5f);
        glm::vec3 specular = color * glm::vec3(1.0f);

        shader.setVec3("spotLight.position", glm::value_ptr(position));
        shader.setVec3("spotLight.direction", glm::value_ptr(direction));
        shader.setFloat("spotLight.cutoff", cutoff);
        shader.setFloat("spotLight.outerCutoff", outerCutoff);
        shader.setFloat("spotLight.attenuationFactor1", attenuationFactor1);
        shader.setFloat("spotLight.attenuationFactor2", attenuationFactor2);
        shader.setFloat("spotLight.attenuationFactor3", attenuationFactor3);
        shader.setVec3("spotLight.ambient", glm::value_ptr(ambient));
        shader.setVec3("spotLight.diffuse", glm::value_ptr(diffuse));
        shader.setVec3("spotLight.specular", glm::value_ptr(specular));
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
        matrix = glm::scale(matrix, glm::vec3(0.1f));
    };
};

#endif