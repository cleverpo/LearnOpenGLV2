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
#include "../../common/models/Shape.h"

#include <iostream>
#include <math.h>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//shader文件路径
const char* LIGHTING_VS_SHADER_PATH = "../src/4_1_depth_test/shader/vert.vs";
const char* LIGHTING_FS_SHADER_PATH = "../src/4_1_depth_test/shader/frag.fs";

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
    glViewport(0, 0, 800, 600);
    //开启深度测试
    glEnable(GL_DEPTH_TEST);

    //相机
    camera.setPosition(glm::vec3(0.0f, 0.0f, 4.0f));
    camera.setClippingPlane(0.1f, 10.f);
    
    //普通物体着色器
    Shader shader(LIGHTING_VS_SHADER_PATH, LIGHTING_FS_SHADER_PATH);
    shader.use();
    shader.setFloat("shininess", 32.0);

    //光源
    //方向光
    DirectionLightParameters dirLightParams;
    dirLightParams.position = glm::vec3(1.2f, 1.0f, 2.0f);
    dirLightParams.direction = glm::vec3(-1.0f, -0.2f, -0.5f);
    dirLightParams.color = glm::vec3(1.0f, 1.0f, 1.0f);
    DirectionLight dirLight(dirLightParams);
    
    glm::vec3 lightAmbient, lightDiffuse, lightSpecular;
    dirLight.getLightColor(lightAmbient, lightDiffuse, lightSpecular);

    shader.setVec3("directionLight.direction", glm::value_ptr(dirLight.direction));
    shader.setVec3("directionLight.ambient", glm::value_ptr(lightAmbient));
    shader.setVec3("directionLight.diffuse", glm::value_ptr(lightDiffuse));
    shader.setVec3("directionLight.specular", glm::value_ptr(lightSpecular));
    
    //创建模型
    Shape cube1(CUBE, "../texture/container2.png");
    Shape cube2(CUBE, "../texture/container2.png");
    Shape plane1(PLANE, "../texture/wall.jpg");

    glm::mat4 matCube1 = glm::mat4(1.0f);
    glm::mat4 matCube2 = glm::mat4(1.0f);
    glm::mat4 matPlane1 = glm::mat4(1.0f);
    
    matCube1 = glm::translate(matCube1, glm::vec3(0.0f, 0.0f, 0.0f));
    matCube2 = glm::translate(matCube2, glm::vec3(1.0f, 0.0f, 1.0f));
    matPlane1 = glm::translate(matPlane1, glm::vec3(0.0f, 0.0f, 0.0f));

    //渲染循环
    while(!glfwWindowShouldClose(window)){
        //获取用户交互输入
        processInput(window);

        //渲染指令
        //清空屏幕
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        dirLight.render(camera);
        
        shader.use();
        shader.setMat4("view", glm::value_ptr(camera.matView));
        shader.setMat4("projection", glm::value_ptr(camera.matProjection));

        //camera
        shader.setVec3("camera.position", glm::value_ptr(camera.position));
        shader.setFloat("camera.near", camera.near);
        shader.setFloat("camera.far", camera.far);

        shader.setMat4("model", glm::value_ptr(matCube1));
        cube1.draw(shader);

        shader.setMat4("model", glm::value_ptr(matCube2));
        cube2.draw(shader);
        
        shader.setMat4("model", glm::value_ptr(matPlane1));
        plane1.draw(shader);

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