#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

enum CameraMovement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

enum CameraProjectionMode{
    PERSPECTIVE,
    ORTHO,
};

class Camera{
public:
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;

    CameraProjectionMode mode;

    float pitch = 0.0f;
    float yaw = -90.0f;

    float fov = 90.0f;
    float aspect = 800.0f/600.0f;

    float near = 0.1f;
    float far = 100.0f;

    glm::mat4 matView;
    glm::mat4 matViewSkybox;  //忽略相机位置，永远都在0，0，0位置
    glm::mat4 matProjection;

    Camera():
    position(glm::vec3(0.0f, 0.0f, 3.0f)),
    forward(glm::vec3(0.0f, 0.0f, -1.0f)),
    up(glm::vec3(0.0f, 1.0f, 0.0f))
    {
        updateViewMatrix();
        updateProjectionMatrix();
    }

    void setPosition(const glm::vec3& pos){
        position = pos;
        updateViewMatrix();
    }

    void setForward(const glm::vec3& fwd){
        forward = fwd;
        updateViewMatrix();
    }

    void setUp(const glm::vec3& u){
        up = u;
        updateViewMatrix();
    }

    void setFov(float f){
        fov = f;
        updateProjectionMatrix();
    }

    void setClippingPlane(float near, float far){
        this->near = near;
        this->far = far;
        updateProjectionMatrix();
    }

    //处理键盘输入，改变摄像机位置
    void processKeyboardInput(const CameraMovement& direction, float deltaTime){
        float speed = 2.5f * deltaTime;
        if(direction == FORWARD){
            position += speed * forward;
        }else if(direction == BACKWARD){
            position -= speed * forward;
        }else if(direction == LEFT){
            position -= speed * glm::normalize(glm::cross(forward, up));
        }else if(direction == RIGHT){
            position += speed * glm::normalize(glm::cross(forward, up));
        }
        updateViewMatrix();
    }

    //处理鼠标输入，改变视角（仰俯角和偏航角）
    void processMouseInput(float xoffset, float yoffset){
        pitch += yoffset;
        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;
        yaw += xoffset;

        forward = glm::normalize(
            glm::vec3(
                cos(glm::radians(pitch)) * cos(glm::radians(yaw)), 
                sin(glm::radians(pitch)), 
                cos(glm::radians(pitch)) * sin(glm::radians(yaw))
            )
        );
        updateViewMatrix();
    }

    //处理滚轮输入，改变fov，进而改变缩放
    void processScrollInput(float yoffset){
        fov -= yoffset;
        if(fov < 1.0f)
            fov = 1.0f;
        if(fov > 89.0f)
            fov = 89.0f;
        updateProjectionMatrix();
    }

private:
    void updateViewMatrix(){
        matView = lookAt(position, position + forward, up);

        matViewSkybox = matView;
        matViewSkybox[3][0] = 0.0f;
        matViewSkybox[3][1] = 0.0f;
        matViewSkybox[3][2] = 0.0f;
    }

    void updateProjectionMatrix(){

        // matProjection = glm::perspective(glm::radians(fov), aspect, near, far);

        glm::mat4 mat = glm::mat4();
        float tanHalfFov = std::tan(glm::radians(fov) / 2.0f);
        mat[0][0] = 1.0f / (tanHalfFov * aspect);
        mat[1][1] = 1.0f / tanHalfFov;
        mat[2][2] = (near + far)/(near - far);
        mat[3][2] = (2 * near * far)/(near - far);
        mat[2][3] = -1;

        matProjection = mat;
    }

    glm::mat4 lookAt(const glm::vec3& posWS, const glm::vec3& targetPos, const glm::vec3& upWS){
        // return glm::lookAt(posWS, targetPos, upWS);

        //摄像机的朝向
        glm::vec3 forward = normalize(targetPos - posWS);
        //因为摄像机朝向是负z方向
        glm::vec3 back = -forward;
        //右手坐标系
        glm::vec3 right = glm::normalize(glm::cross(upWS, back));
        //right和-forward叉乘得到up
        glm::vec3 up = glm::normalize(glm::cross(back, right));
        
        //opengl矩阵是列矩阵
        //view->world平移
        glm::mat4 trans(1.0f);
        trans[3][0] = -posWS.x;
        trans[3][1] = -posWS.y;
        trans[3][2] = -posWS.z;

        //正交矩阵
        glm::mat4 ortho(1.0f);
        ortho[0][0] = right.x; 
        ortho[0][1] = up.x;
        ortho[0][2] = back.x;
        
        ortho[1][0] = right.y; 
        ortho[1][1] = up.y;
        ortho[1][2] = back.y;
        
        ortho[2][0] = right.z; 
        ortho[2][1] = up.z;
        ortho[2][2] = back.z;
        
        glm::mat4 lookat = ortho * trans;
        
        return lookat;
    }
};

#endif