#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "../../common/Shader.h"
#include "../../common/Texture2D.h"
#include "../../common/Camera.h"

#include <iostream>
#include <math.h>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera camera;

//shader文件路径
const char* LIGHTING_VS_SHADER_PATH = "../src/2_1_color/shader/vert.vs";
const char* LIGHTING_FS_SHADER_PATH = "../src/2_1_color/shader/frag.fs";

const char* COMMON_LIGHT_VS_SHADER_PATH = "../src/common/shader/light_vert.vs";
const char* COMMON_LIGHT_FS_SHADER_PATH = "../src/common/shader/light_frag.fs";

//纹理数据
const int TEXTURE_NUM = 0;
const std::string TEXTURE_PATH[TEXTURE_NUM] = {
    // "../texture/container.jpg",
    // "../texture/awesomeface.png"
};
const Texture2DParameters TEXTURE_PARAMETERS[TEXTURE_NUM] = {
    // Texture2DParameters(GL_RGB, GL_RGB, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, true),
    // Texture2DParameters(GL_RGBA, GL_RGBA, GL_MIRRORED_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, true)
};

//顶点数据
//立方体
float VERTICES[] = {
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

    //创建顶点缓冲对象
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    //绑定缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //把顶点数据复制到缓冲对象
    glBufferData(GL_ARRAY_BUFFER, sizeof(VERTICES), VERTICES, GL_STATIC_DRAW);

    //光源的显示
    //光源着色器
    Shader lightShader(COMMON_LIGHT_VS_SHADER_PATH, COMMON_LIGHT_FS_SHADER_PATH);
    //光源VAO
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //光源颜色
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

    //光源的位置
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);
    lightModel = glm::scale(lightModel, glm::vec3(0.2f));

    //普通物体着色器
    Shader lightingShader(LIGHTING_VS_SHADER_PATH, LIGHTING_FS_SHADER_PATH);
    //普通物体VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    //物体颜色
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);

    //物体位置
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);

    //线框模式
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //普通模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    //camera初始化
    camera.setFov(90.f);

    //渲染循环
    while(!glfwWindowShouldClose(window)){
        //获取用户交互输入
        processInput(window);

        //渲染指令
        //清空屏幕
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //开启深度测试
        glEnable(GL_DEPTH_TEST);
        
        //光源
        lightShader.use();
        lightShader.setMat4("model", glm::value_ptr(lightModel));
        lightShader.setMat4("view", glm::value_ptr(camera.matView));
        lightShader.setMat4("projection", glm::value_ptr(camera.matProjection));
        lightShader.setVec3("lightColor", glm::value_ptr(lightColor));

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        lightingShader.use();
        lightingShader.setMat4("model", glm::value_ptr(model));
        lightingShader.setMat4("view", glm::value_ptr(camera.matView));
        lightingShader.setMat4("projection", glm::value_ptr(camera.matProjection));
        lightingShader.setVec3("lightColor", glm::value_ptr(lightColor));
        lightingShader.setVec3("objectColor", glm::value_ptr(objectColor));

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        //交换缓冲区
        glfwSwapBuffers(window);
        //检查有没有触发什么事件,比如键盘输入、鼠标移动
        glfwPollEvents();
    }

    //释放纹理
    // for(int i = 0; i < TEXTURE_NUM; i++){
    //     delete textures[i];
    // }

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