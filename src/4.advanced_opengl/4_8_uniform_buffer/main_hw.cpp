#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../common/Material.h"
#include "../../common/Shader.h"
#include "../../common/Camera.h"
#include "../../common/DirectionLight.h"
#include "../../common/PointLight.h"
#include "../../common/SpotLight.h"
#include "../../common/Mesh.h"
#include "../../common/models/CustomShape.h"
#include "../../common/models/Shape.h"
#include "../../common/Model.h"

#include "../../common/TextureCube.h"
#include "../../common/Skybox.h"

#include <iostream>
#include <math.h>
#include <string>
#include <map>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//shader文件路径
const char* COLOR_VS_SHADER_PATH = "../src/4_8_uniform_buffer/shader/vert.vs";
const char* COLOR_RED_FS_SHADER_PATH = "../src/4_8_uniform_buffer/shader/frag_red.fs";
const char* COLOR_GREEN_FS_SHADER_PATH = "../src/4_8_uniform_buffer/shader/frag_green.fs";
const char* COLOR_BLUE_FS_SHADER_PATH = "../src/4_8_uniform_buffer/shader/frag_blue.fs";
const char* COLOR_YELLOW_FS_SHADER_PATH = "../src/4_8_uniform_buffer/shader/frag_yellow.fs";

const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

Camera camera;

int main(){
    //初始化GLFW
    glfwInit();
    //配置GLFW
    //设置主要和次要版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //设置使用核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //创建窗口对象
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if(window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    //通知GLFW将我们窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);
    
    //注册窗口大小改变的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //鼠标输入配置
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //注册鼠标移动的回调函数
    glfwSetCursorPosCallback(window, mouse_callback);
    //注册滚轮滚动的回调函数
    glfwSetScrollCallback(window, scroll_callback);

    //初始化GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //设置视口
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    //开启深度测试
    glEnable(GL_DEPTH_TEST);

    //相机
    camera.setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    camera.setClippingPlane(0.1f, 100.f);

    //普通物体着色器
    Shader shaderRed(COLOR_VS_SHADER_PATH, COLOR_RED_FS_SHADER_PATH);
    Shader shaderGreen(COLOR_VS_SHADER_PATH, COLOR_GREEN_FS_SHADER_PATH);
    Shader shaderBlue(COLOR_VS_SHADER_PATH, COLOR_BLUE_FS_SHADER_PATH);
    Shader shaderYellow(COLOR_VS_SHADER_PATH, COLOR_YELLOW_FS_SHADER_PATH);
    
    //创建模型
    Shape cube(CUBE, NULL);

    glm::mat4 matCube1(1.0);
    matCube1 = glm::translate(matCube1, glm::vec3(-2.0, 2.0, 0.0));
    shaderRed.use();
    shaderRed.setMat4("model", glm::value_ptr(matCube1));
    
    glm::mat4 matCube2(1.0);
    matCube2 = glm::translate(matCube2, glm::vec3(-2.0, -2.0, 0.0));
    shaderGreen.use();
    shaderGreen.setMat4("model", glm::value_ptr(matCube2));
    
    glm::mat4 matCube3(1.0);
    matCube3 = glm::translate(matCube3, glm::vec3(2.0, -2.0, 0.0));
    shaderBlue.use();
    shaderBlue.setMat4("model", glm::value_ptr(matCube3));
    
    glm::mat4 matCube4(1.0);
    matCube4 = glm::translate(matCube4, glm::vec3(2.0, 2.0, 0.0));
    shaderYellow.use();
    shaderYellow.setMat4("model", glm::value_ptr(matCube4));

    //创建uniform缓冲
    unsigned int uboMatrices;
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    //分配空间
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);

    //着色器中uniform matrices绑定绑定点0
    unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(shaderRed.ID, "Matrices");
    unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen.ID, "Matrices");
    unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue.ID, "Matrices");
    unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow.ID, "Matrices");

    glUniformBlockBinding(shaderRed.ID, uniformBlockIndexRed, 0);
    glUniformBlockBinding(shaderGreen.ID, uniformBlockIndexGreen, 0);
    glUniformBlockBinding(shaderBlue.ID, uniformBlockIndexBlue, 0);
    glUniformBlockBinding(shaderYellow.ID, uniformBlockIndexYellow, 0);

    //uniform缓冲绑定绑定点0
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

    //填充uniform缓冲数据
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(camera.matProjection), glm::value_ptr(camera.matProjection));
    //完成
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //渲染循环
    while(!glfwWindowShouldClose(window)){
        //获取用户交互输入
        processInput(window);

        //渲染指令
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //填充uniform缓冲数据
        glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(camera.matProjection), sizeof(camera.matView), glm::value_ptr(camera.matView));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        shaderRed.use();
        cube.draw(shaderRed);

        shaderGreen.use();
        cube.draw(shaderGreen);
        
        shaderBlue.use();
        cube.draw(shaderBlue);
        
        shaderYellow.use();
        cube.draw(shaderYellow);

        //交换缓冲区
        glfwSwapBuffers(window);
        //检查有没有触发什么事件,比如键盘输入、鼠标移动
        glfwPollEvents();
    }

    //释放/删除之前分配的所有资源
    glfwTerminate();

    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

float lastTime = 0.0f;
void processInput(GLFWwindow* window){
    //按下esc键,关闭窗口
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }

    float curTime = glfwGetTime();
    float deltaTime = curTime - lastTime;
    lastTime = curTime;
    
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.processKeyboardInput(FORWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.processKeyboardInput(BACKWARD, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.processKeyboardInput(LEFT, deltaTime);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.processKeyboardInput(RIGHT, deltaTime);
    }
}

float isFristMouse = true;
float lastMouseX = 400, lastMouseY = 300;
void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(isFristMouse){
        lastMouseX = xpos;
        lastMouseY = ypos;
        isFristMouse = false;
    }
    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos;
    lastMouseX = xpos;
    lastMouseY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    camera.processMouseInput(xoffset, yoffset);
}

float lastScrollY = 0;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
    camera.processScrollInput(yoffset - lastScrollY);
}