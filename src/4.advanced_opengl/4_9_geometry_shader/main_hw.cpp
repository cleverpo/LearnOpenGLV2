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

#define GRASS_TEST 0
#define EXPLODE_TEST 0
#define DEBUG_NORMAL_TEST 1

//shader文件路径
#if GRASS_TEST
const char* GRASS_VS_SHADER_PATH = "../src/4_9_geometry_shader/shader/vert_grass.vs";
const char* GRASS_FS_SHADER_PATH = "../src/4_9_geometry_shader/shader/frag_grass.fs";
const char* GRASS_GS_SHADER_PATH = "../src/4_9_geometry_shader/shader/geo_grass.gs";
#elif EXPLODE_TEST
const char* EXPLODE_VS_SHADER_PATH = "../src/4_9_geometry_shader/shader/vert_explode.vs";
const char* EXPLODE_FS_SHADER_PATH = "../src/4_9_geometry_shader/shader/frag_explode.fs";
const char* EXPLODE_GS_SHADER_PATH = "../src/4_9_geometry_shader/shader/geo_explode.gs";
#elif DEBUG_NORMAL_TEST
const char* LIT_VS_SHADER_PATH = "../src/common/shader/simple_lit_vert.vs";
const char* LIT_FS_SHADER_PATH = "../src/common/shader/simple_lit_frag.fs";
const char* DEBUG_NORMAL_VS_SHADER_PATH = "../src/common/shader/debug/debug_normal_vert.vs";
const char* DEBUG_NORMAL_FS_SHADER_PATH = "../src/common/shader/debug/debug_normal_frag.fs";
const char* DEBUG_NORMAL_GS_SHADER_PATH = "../src/common/shader/debug/debug_normal_geo.gs";
#endif

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

    //着色器
#if GRASS_TEST
    Shader shader(GRASS_VS_SHADER_PATH, GRASS_FS_SHADER_PATH, GRASS_GS_SHADER_PATH);
#elif EXPLODE_TEST
    Shader shader(EXPLODE_VS_SHADER_PATH, EXPLODE_FS_SHADER_PATH, EXPLODE_GS_SHADER_PATH);
#elif DEBUG_NORMAL_TEST
    Shader litShader(LIT_VS_SHADER_PATH, LIT_FS_SHADER_PATH);
    Shader shader(DEBUG_NORMAL_VS_SHADER_PATH, DEBUG_NORMAL_FS_SHADER_PATH, DEBUG_NORMAL_GS_SHADER_PATH);
#endif

    //创建模型
#if GRASS_TEST
    Shape quad(QUAD, NULL);

    glm::mat4 matQuad1(1.0);
    matQuad1 = glm::rotate(matQuad1, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.use();
    shader.setMat4("model", glm::value_ptr(matQuad1));
#elif EXPLODE_TEST
    Model model("../model/nanosuit/nanosuit.obj");
    
    glm::mat4 matModel(1.0);
    matModel = glm::scale(matModel, glm::vec3(0.2f));
    shader.use();
    shader.setMat4("model", glm::value_ptr(matModel));
#elif DEBUG_NORMAL_TEST
    Model model("../model/nanosuit/nanosuit.obj");
    
    glm::mat4 matModel(1.0);
    matModel = glm::scale(matModel, glm::vec3(0.2f));
    shader.use();
    shader.setMat4("model", glm::value_ptr(matModel));
    litShader.use();
    litShader.setMat4("model", glm::value_ptr(matModel));

#endif

    //创建光
#if EXPLODE_TEST
    DirectionLightParameters litParams;
    litParams.direction = normalize(glm::vec3(-1.0f));
    litParams.color = glm::vec3(1.0f, 1.0f, 1.0f);

    DirectionLight light(litParams);
    light.apply(shader);
#elif DEBUG_NORMAL_TEST
    DirectionLightParameters litParams;
    litParams.direction = normalize(glm::vec3(-1.0f));
    litParams.color = glm::vec3(1.0f, 1.0f, 1.0f);

    DirectionLight light(litParams);
    light.apply(litShader);

    litShader.setInt("pointLightCount", 0);
#endif

    //渲染循环
    while(!glfwWindowShouldClose(window)){
        //获取用户交互输入
        processInput(window);

        //渲染指令
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //填充uniform缓冲数据
        shader.use();
        shader.setMat4("view", glm::value_ptr(camera.matView));
        shader.setMat4("projection", glm::value_ptr(camera.matProjection));

#if GRASS_TEST
        quad.draw(shader);
#elif EXPLODE_TEST
        shader.setVec3("cameraPos", glm::value_ptr(camera.position));
        shader.setFloat("time", glfwGetTime());
        model.draw(shader);
#elif DEBUG_NORMAL_TEST
        litShader.use();
        litShader.setMat4("view", glm::value_ptr(camera.matView));
        litShader.setMat4("projection", glm::value_ptr(camera.matProjection));
        litShader.setVec3("cameraPos", glm::value_ptr(camera.position));
        model.draw(litShader);

        shader.use();
        model.draw(shader);
#endif

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