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
const char* LIGHTING_VS_SHADER_PATH = "../src/4_6_cubemap/shader/vert.vs";
const char* LIGHTING_FS_SHADER_PATH = "../src/4_6_cubemap/shader/frag.fs";

const char* REFLECT_VS_SHADER_PATH = "../src/4_6_cubemap/shader/vert.vs";
const char* REFLECT_FS_SHADER_PATH = "../src/4_6_cubemap/shader/frag_reflect.fs";

const char* REFRACT_VS_SHADER_PATH = "../src/4_6_cubemap/shader/vert.vs";
const char* REFRACT_FS_SHADER_PATH = "../src/4_6_cubemap/shader/frag_refract.fs";

const char* SKYBOX_VS_SHADER_PATH = "../src/4_6_cubemap/shader/vert_skybox.vs";
const char* SKYBOX_FS_SHADER_PATH = "../src/4_6_cubemap/shader/frag_skybox.fs";

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
    Shader shader(LIGHTING_VS_SHADER_PATH, LIGHTING_FS_SHADER_PATH);
    //反射物体着色器
    Shader reflectShader(REFLECT_VS_SHADER_PATH, REFLECT_FS_SHADER_PATH);
    //折射物体着色器
    Shader refractShader(REFRACT_VS_SHADER_PATH, REFRACT_FS_SHADER_PATH);
    //天空和着色器
    Shader skyboxShader(SKYBOX_VS_SHADER_PATH, SKYBOX_FS_SHADER_PATH);
    
    //光源
    //方向光
    DirectionLightParameters dirLightParams;
    dirLightParams.position = glm::vec3(1.2f, 1.0f, 2.0f);
    dirLightParams.direction = glm::vec3(-1.0f, -0.2f, -0.5f);
    dirLightParams.color = glm::vec3(1.0f, 1.0f, 1.0f);
    DirectionLight dirLight(dirLightParams);
    
    glm::vec3 lightAmbient, lightDiffuse, lightSpecular;
    dirLight.getLightColor(lightAmbient, lightDiffuse, lightSpecular);

    shader.use();
    shader.setVec3("directionLight.direction", glm::value_ptr(dirLight.direction));
    shader.setVec3("directionLight.ambient", glm::value_ptr(lightAmbient));
    shader.setVec3("directionLight.diffuse", glm::value_ptr(lightDiffuse));
    shader.setVec3("directionLight.specular", glm::value_ptr(lightSpecular));
    
    //创建模型
    Model nanosuit("../model/nanosuit/nanosuit.obj");

    glm::mat4 matModelNanosuit1(1.0);
    matModelNanosuit1 = glm::translate(matModelNanosuit1, glm::vec3(-2.0, -2.0, 0.0));
    matModelNanosuit1 = glm::scale(matModelNanosuit1, glm::vec3(0.2));
    reflectShader.use();
    reflectShader.setMat4("model", glm::value_ptr(matModelNanosuit1));
    reflectShader.setInt("environmentMapping", 0);
    
    glm::mat4 matModelNanosuit2(1.0);
    matModelNanosuit2 = glm::translate(matModelNanosuit2, glm::vec3(2.0, -2.0, 0.0));
    matModelNanosuit2 = glm::scale(matModelNanosuit2, glm::vec3(0.2));
    refractShader.use();
    refractShader.setMat4("model", glm::value_ptr(matModelNanosuit2));
    refractShader.setInt("environmentMapping", 0);
    
    glm::mat4 matModelNanosuit3(1.0);
    matModelNanosuit3 = glm::translate(matModelNanosuit3, glm::vec3(0.0, -2.0, 0.0));
    matModelNanosuit3 = glm::scale(matModelNanosuit3, glm::vec3(0.2));
    shader.use();
    shader.setMat4("model", glm::value_ptr(matModelNanosuit3));
    shader.setInt("environmentMapping", 4);

    //天空盒
    vector<string> skyboxTextures;
    skyboxTextures.push_back("../texture/skybox/right.jpg");
    skyboxTextures.push_back("../texture/skybox/left.jpg");
    skyboxTextures.push_back("../texture/skybox/top.jpg");
    skyboxTextures.push_back("../texture/skybox/bottom.jpg");
    skyboxTextures.push_back("../texture/skybox/front.jpg");
    skyboxTextures.push_back("../texture/skybox/back.jpg");
    TextureCubeParameters texParams(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR);
    Skybox skybox(skyboxTextures, texParams);

    //渲染循环
    while(!glfwWindowShouldClose(window)){
        //获取用户交互输入
        processInput(window);

        //渲染指令
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        dirLight.render(camera);
        
        //更新shader
        shader.use();
        //set uniform
        shader.setFloat("shininess", 32.0);
        shader.setMat4("view", glm::value_ptr(camera.matView));
        shader.setMat4("projection", glm::value_ptr(camera.matProjection));
        shader.setVec3("camera.position", glm::value_ptr(camera.position));
        shader.setFloat("camera.near", camera.near);
        shader.setFloat("camera.far", camera.far);

        reflectShader.use();
        reflectShader.setFloat("shininess", 32.0);
        reflectShader.setMat4("view", glm::value_ptr(camera.matView));
        reflectShader.setMat4("projection", glm::value_ptr(camera.matProjection));
        reflectShader.setVec3("camera.position", glm::value_ptr(camera.position));
        reflectShader.setFloat("camera.near", camera.near);
        reflectShader.setFloat("camera.far", camera.far);
        
        refractShader.use();
        refractShader.setFloat("shininess", 32.0);
        refractShader.setMat4("view", glm::value_ptr(camera.matView));
        refractShader.setMat4("projection", glm::value_ptr(camera.matProjection));
        refractShader.setVec3("camera.position", glm::value_ptr(camera.position));
        refractShader.setFloat("camera.near", camera.near);
        refractShader.setFloat("camera.far", camera.far);

        skyboxShader.use();
        glm::mat4 matViewSkybox = glm::mat4(glm::mat3(camera.matView));
        skyboxShader.setMat4("view", glm::value_ptr(matViewSkybox));
        skyboxShader.setMat4("projection", glm::value_ptr(camera.matProjection));

        //场景绘制
        // skybox.textuerCube->use(0);
        // reflectShader.use();
        // nanosuit.draw(reflectShader);
        
        // refractShader.use();
        // nanosuit.draw(refractShader);

        skybox.textuerCube->use(4);
        shader.use();
        nanosuit.draw(shader);

        //天空盒绘制（最后绘制，减少overdraw)
        skyboxShader.use();
        skybox.draw(skyboxShader);

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