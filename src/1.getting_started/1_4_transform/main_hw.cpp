#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include "../../../common/Shader.h"
#include "../../../common/Texture2D.h"

#include <iostream>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

float mixValue = 0.2f;

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

    //初始化GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //设置视口
    glViewport(0, 0, 800, 600);
    //注册窗口大小改变的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //着色器
    Shader shader("../src/1_4_transform/shader/vert.vs", "../src/1_4_transform/shader/frag.fs");

    //纹理
    Texture2D texture1("../texture/container.jpg", Texture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, GL_RGB, true));
    Texture2D texture2("../texture/awesomeface.png", Texture2DParameters(GL_MIRRORED_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA, true));
    
    shader.use();
    shader.setInt("texture0", 0);
    shader.setInt("texture1", 1);

    //矩形
    float vertices[] = {
        //position  color  uv
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, //右上角 
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, //右下角
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, //左下角
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, //左上角
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    //绑定顶点数组对象
    glBindVertexArray(VAO);

    //创建顶点缓冲对象
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    //绑定缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //把顶点数据复制到缓冲对象
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //创建索引缓冲对象
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    //绑定索引缓冲对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //把索引数据复制到缓冲对象
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //设置顶点属性指针
    /**
     * 位置属性(对应shader里location)
     * 数量
     * 是否normalize
     * 步长
     * 偏移
     **/
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); 
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); 
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

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
        glClear(GL_COLOR_BUFFER_BIT);

        texture1.use(0);
        texture2.use(1);
        

        shader.use();
        shader.setFloat("mixValue", mixValue);

        glBindVertexArray(VAO);

        //矩阵1，右下角，自旋转
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0));
        trans = glm::rotate(trans, (float)glfwGetTime() * 0.5f, glm::vec3(0.0, 0.0, 1.0));
        shader.setMat4("transform", glm::value_ptr(trans));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        //矩阵2，左上角，自缩放
        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0));

        float scaleAmount = sin(glfwGetTime()) * 0.5f + 0.5f;
        trans = glm::scale(trans, glm::vec3(scaleAmount, scaleAmount, 1.0f));
        shader.setMat4("transform", glm::value_ptr(trans));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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

void processInput(GLFWwindow* window){
    //按下esc键,关闭窗口
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        mixValue += 0.001f;
        if(mixValue >= 1.0f){
            mixValue = 1.0f;
        }
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        mixValue -= 0.001f;
        if(mixValue <= 0.0f){
            mixValue = 0.0f;
        }
    }
}