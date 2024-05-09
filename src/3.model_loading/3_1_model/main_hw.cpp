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
#include "../../common/Model.h"

#include <iostream>
#include <math.h>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera camera;

//shader文件路径
const char* LIGHTING_VS_SHADER_PATH = "../src/3_1_model/shader/vert.vs";
const char* LIGHTING_FS_SHADER_PATH = "../src/3_1_model/shader/frag.fs";

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
    dirLight.apply(shader);
    
    //点光源
    shader.setInt("pointLightCount", 4);
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
    };
    PointLightParameters pointLightParams;
    PointLight* pointLights[4];
    for(unsigned int i = 0; i < 4; i++){
        pointLightParams.color = glm::vec3(1.0, 1.0f, 1.0f);
        pointLightParams.attenuationFactor1 = 1.0f;
        pointLightParams.attenuationFactor2 = 0.09f;
        pointLightParams.attenuationFactor3 = 0.032f;
        pointLightParams.position = pointLightPositions[i];

        PointLight* light = new PointLight(pointLightParams);
        pointLights[i] = light;
        
        light->apply(shader, i);
    }

    //创建模型
    Model model("../model/nanosuit/nanosuit.obj");
    Model model1("../model/backpack/backpack.obj");

    glm::mat4 matModel = glm::mat4(1.0f);
    glm::mat4 matModel1 = glm::mat4(1.0f);
    
    matModel = glm::translate(matModel, glm::vec3(0.0f, 0.0f, 0.0f));
    matModel = glm::scale(matModel, glm::vec3(0.1f, 0.1f, 0.1f));

    matModel1 = glm::translate(matModel1, glm::vec3(1.0f, 0.0f, 0.0f));
    matModel1 = glm::scale(matModel1, glm::vec3(0.1f, 0.1f, 0.1f));

    //渲染循环
    while(!glfwWindowShouldClose(window)){
        //获取用户交互输入
        processInput(window);

        //渲染指令
        //清空屏幕
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        dirLight.render(camera);
        for(int i = 0; i < 4; i++){
            pointLights[i]->render(camera);
        }
        
        shader.use();
        shader.setMat4("model", glm::value_ptr(matModel));
        shader.setMat4("view", glm::value_ptr(camera.matView));
        shader.setMat4("projection", glm::value_ptr(camera.matProjection));
        shader.setVec3("cameraPos", glm::value_ptr(camera.position));

        model.draw(shader);

        shader.setMat4("model", glm::value_ptr(matModel1));
        model1.draw(shader);

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