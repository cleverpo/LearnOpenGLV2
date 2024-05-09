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
#include "../../common/debug/DebugUtils.h"

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
const char* VS_SHADER_PATH = "../src/5_3_1_shadow_mapping/shader/vert.vs";
const char* FS_SHADER_PATH = "../src/5_3_1_shadow_mapping/shader/frag.fs";

const char* SHADOW_MAPPING_VS_SHADER_PATH = "../src/common/shader/directional_shadow_mapping_vert.vs";
const char* SHADOW_MAPPING_FS_SHADER_PATH = "../src/common/shader/directional_shadow_mapping_frag.fs";

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
    //剔除
    glEnable(GL_CULL_FACE);

    //相机
    camera.setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    camera.setClippingPlane(0.1f, 100.f);

    //着色器
    Shader shader(VS_SHADER_PATH, FS_SHADER_PATH, NULL);
    Shader shadowMappingShader(SHADOW_MAPPING_VS_SHADER_PATH, SHADOW_MAPPING_FS_SHADER_PATH, NULL);

    //创建模型
    //地板
    Shape plane(PLANE, "../texture/wood.png");
    glm::mat4 matModelPlane(1.0);

    Shape cube(CUBE, "../texture/container.jpg");
    Shape cubeBg(CUBE, "../texture/wood.png");
    glm::mat4 matModelCubes[3];
    glm::mat4 matModelCubeBg;
    matModelCubes[0] = glm::mat4(1.0);
    matModelCubes[0] = glm::translate(matModelCubes[0], glm::vec3(0.0f, 1.5f, 0.0));

    matModelCubes[1] = glm::mat4(1.0);
    matModelCubes[1] = glm::translate(matModelCubes[1], glm::vec3(2.0f, 0.0f, 1.0));

    matModelCubes[2] = glm::mat4(1.0);
    matModelCubes[2] = glm::translate(matModelCubes[2], glm::vec3(-1.0f, 0.0f, 2.0));
    matModelCubes[2] = glm::rotate(matModelCubes[2], glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    matModelCubes[2] = glm::scale(matModelCubes[2], glm::vec3(0.5));

    
    matModelCubeBg = glm::mat4(1.0);
    matModelCubeBg = glm::translate(matModelCubeBg, glm::vec3(0.0f, -0.5f, 0.0));
    matModelCubeBg = glm::scale(matModelCubeBg, glm::vec3(1.0f, 0.25f, 1.0f));

    //光照
    DirectionLightParameters lightParam;
    lightParam.position = glm::vec3(-2.0f, 4.0f, -1.0f);
    lightParam.color = glm::vec3(1.0f);
    lightParam.direction = glm::vec3(-1.0f, -1.0f, 0.0f);

    DirectionLight light(lightParam);
    shader.use();
    light.apply(shader);

    shader.setInt("pointLightCount", 0);
    shader.setFloat("shininess", 64.0);

    //阴影
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    //阴影帧缓冲
    unsigned int shadowMappingFBO;
    glGenFramebuffers(1, &shadowMappingFBO);
    //深度纹理深度附件
    unsigned int depthMapTex;
    glGenTextures(1, &depthMapTex);
    glBindTexture(GL_TEXTURE_2D, depthMapTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMappingFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //渲染循环
    while(!glfwWindowShouldClose(window)){
        //获取用户交互输入
        processInput(window);

        //渲染深度贴图
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMappingFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        // glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);

        shadowMappingShader.use();
        light.applyLightSpaceMatrix(shadowMappingShader);
        shadowMappingShader.setMat4("model", glm::value_ptr(matModelPlane));

        plane.draw(shadowMappingShader);
        
        for(unsigned int i = 0; i < 3; i++){
            shadowMappingShader.setMat4("model", glm::value_ptr(matModelCubes[i]));
            cube.draw(shadowMappingShader);
        }

        shadowMappingShader.setMat4("model", glm::value_ptr(matModelCubeBg));
        cubeBg.draw(shadowMappingShader);

        //正常渲染
        // glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        // glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // drawDebugTexture(depthMapTex);
        
        //正常渲染
        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_BACK);

        light.render(camera);

        //填充uniform缓冲数据
        shader.use();
        shader.setMat4("view", glm::value_ptr(camera.matView));
        shader.setMat4("projection", glm::value_ptr(camera.matProjection));
        shader.setVec3("cameraPos", glm::value_ptr(camera.position));
        shader.setMat4("model", glm::value_ptr(matModelPlane));
        shader.setInt("depthMap", 8);
        glActiveTexture(GL_TEXTURE0 + 8);
        glBindTexture(GL_TEXTURE_2D, depthMapTex);

        light.applyLightSpaceMatrix(shader);

        plane.draw(shader);

        for(unsigned int i = 0; i < 3; i++){
            shader.setMat4("model", glm::value_ptr(matModelCubes[i]));
            cube.draw(shader);
        }
        shader.setMat4("model", glm::value_ptr(matModelCubeBg));
        cubeBg.draw(shader);

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
float isPressed = false;
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

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isPressed){
        isPressed = true;
    }
    
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE){
        isPressed = false;
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