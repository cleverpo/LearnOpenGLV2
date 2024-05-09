#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "../../common/Material.h"
#include "../../common/Shader.h"
#include "../../common/Texture2D.h"
#include "../../common/Camera.h"
#include "../../common/DirectionLight.h"
#include "../../common/PointLight.h"
#include "../../common/SpotLight.h"

#include <iostream>
#include <math.h>
#include <string>

#define DIRECTION_LIGHT 0
#define POINT_LIGHT 0
#define SPOT_LIGHT 1

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

Camera camera;

//shader文件路径
const char* LIGHTING_VS_SHADER_PATH = "../src/2_5_light_casters/shader/vert.vs";
#if DIRECTION_LIGHT
const char* LIGHTING_FS_SHADER_PATH = "../src/2_5_light_casters/shader/frag_direction_light.fs";
#elif POINT_LIGHT
const char* LIGHTING_FS_SHADER_PATH = "../src/2_5_light_casters/shader/frag_point_light.fs";
#elif SPOT_LIGHT
const char* LIGHTING_FS_SHADER_PATH = "../src/2_5_light_casters/shader/frag_spot_light.fs";
#endif

//顶点数据
//立方体
float CUBE_VERTICES[] = {
    //顶点坐标              //法向量                //UV
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
    0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,    1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,    1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,    1.0f, 0.0f,
    0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,    1.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,    0.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,    0.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,    1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
    0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,    1.0f, 1.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
    0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,    1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
    0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,    1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f
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

    //光源
    //方向光
    #if DIRECTION_LIGHT
    DirectionLightParameters lightParams;
    lightParams.position = glm::vec3(1.2f, 1.0f, 2.0f);
    lightParams.direction = glm::vec3(-1.0f, -0.2f, -0.5f);
    lightParams.color = glm::vec3(1.0f, 1.0f, 1.0f);
    DirectionLight light(lightParams);
    #elif POINT_LIGHT
    PointLightParameters lightParams;
    lightParams.position = glm::vec3(1.2f, 1.0f, 2.0f);
    lightParams.color = glm::vec3(1.0f, 1.0f, 1.0f);
    lightParams.attenuationFactor1 = 1.0f;
    lightParams.attenuationFactor2 = 0.09f;
    lightParams.attenuationFactor3 = 0.032f;
    PointLight light(lightParams);
    #elif SPOT_LIGHT
    SpotLightParameters lightParams;
    lightParams.position = camera.position;
    lightParams.direction = camera.forward;
    lightParams.color = glm::vec3(1.0f, 1.0f, 1.0f);
    lightParams.cutoff = cos(glm::radians(6.25f));
    lightParams.outerCutoff = cos(glm::radians(8.75f));
    lightParams.attenuationFactor1 = 1.0f;
    lightParams.attenuationFactor2 = 0.09f;
    lightParams.attenuationFactor3 = 0.032f;
    SpotLight light(lightParams);
    #endif

    //普通物体着色器
    Shader shader(LIGHTING_VS_SHADER_PATH, LIGHTING_FS_SHADER_PATH);
    
    //纹理数据
    Texture2D diffuseMap("../texture/container2.png", Texture2DParameters(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA, true));
    Texture2D specularMap("../texture/container2_specular.png", Texture2DParameters(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA, true));

    //创建顶点缓冲对象
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    //绑定缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //把顶点数据复制到缓冲对象
    glBufferData(GL_ARRAY_BUFFER, sizeof(CUBE_VERTICES), CUBE_VERTICES, GL_STATIC_DRAW);

    //普通物体VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float))); 
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float))); 
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    //物体颜色
    Material material;
    material.diffuse = &diffuseMap;
    material.specular = &specularMap;
    material.shininess = 32.f;

    // positions all containers
    glm::vec3 cubePositions[10] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    //线框模式
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //普通模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

        light.render(camera);
        
        shader.use();
        shader.setMat4("view", glm::value_ptr(camera.matView));
        shader.setMat4("projection", glm::value_ptr(camera.matProjection));

        light.apply(shader);

        material.diffuse->use(0);
        material.specular->use(1);
        shader.setInt("material.diffuse", 0);
        shader.setInt("material.specular", 1);
        shader.setFloat("material.shininess", material.shininess);

        shader.setVec3("cameraPos", glm::value_ptr(camera.position));

        glBindVertexArray(VAO);

        for(int i = 0; i < 10; i++){
            //物体位置
            const glm::vec3& pos = cubePositions[i];
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            //旋转
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            shader.setMat4("model", glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);

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