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
const char* LIGHTING_VS_SHADER_PATH = "../src/4_5_framebuffers/shader/vert.vs";
const char* LIGHTING_FS_SHADER_PATH = "../src/4_5_framebuffers/shader/frag.fs";

const char* SCREEN_VS_SHADER_PATH = "../src/4_5_framebuffers/shader/vert_screen.vs";
const char* SCREEN_FS_SHADER_PATH = "../src/4_5_framebuffers/shader/frag_screen.fs";

const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

const float SCREEN_QUAD[] = {
    //position      uv
    -1.0, 1.0,      0.0, 1.0,
    -1.0, -1.0,      0.0, 0.0,
    1.0, -1.0,       1.0, 0.0,

    -1.0, 1.0,      0.0, 1.0,
    1.0, -1.0,       1.0, 0.0,
    1.0, 1.0,       1.0, 1.0
};

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
    camera.setClippingPlane(0.1f, 10.f);

    //普通物体着色器
    Shader shader(LIGHTING_VS_SHADER_PATH, LIGHTING_FS_SHADER_PATH);
    //屏幕着色器
    Shader screenShader(SCREEN_VS_SHADER_PATH, SCREEN_FS_SHADER_PATH);
    
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
    Shape plane(PLANE, "../texture/wall.jpg");
    Shape box1(CUBE, "../texture/container.jpg");
    Shape box2(CUBE, "../texture/container.jpg");

    glm::mat4 matPlane = glm::mat4(1.0f);
    glm::mat4 matBox1 = glm::mat4(1.0f);
    glm::mat4 matBox2 = glm::mat4(1.0f);
    matBox1 = glm::translate(matBox1, glm::vec3(-1.0f, 0.0f, -1.0f));
    matBox2 = glm::translate(matBox2, glm::vec3(2.0f, 0.0f, -3.0f));

    //创建屏幕quad
    unsigned int screenVAO, screenVBO;

    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);

    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);

    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), SCREEN_QUAD, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //新帧缓冲对象
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    //新建纹理做颜色附件
    unsigned int textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    //新渲染缓冲对象做深度模板附件
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //渲染循环
    while(!glfwWindowShouldClose(window)){
        //获取用户交互输入
        processInput(window);

        //渲染指令
        //清空屏幕
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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

        //场景绘制
        shader.setMat4("model", glm::value_ptr(matPlane));
        plane.draw(shader);
        shader.setMat4("model", glm::value_ptr(matBox1));
        box1.draw(shader);
        shader.setMat4("model", glm::value_ptr(matBox2));
        box2.draw(shader);

        //绘制屏幕后处理图片
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureId);

        screenShader.use();
        screenShader.setVec2("texelSize", glm::value_ptr(glm::vec2(1.0f / SCREEN_WIDTH, 1.0f /  SCREEN_HEIGHT)));
        screenShader.setInt("sreenTexture", 0);

        glBindVertexArray(screenVAO);
        glDrawArrays(GL_TRIANGLES, 0, 24);
        glBindVertexArray(0);

        glBindTexture(GL_TEXTURE_2D, 0);

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