#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../../common/Shader.h"

#include <iostream>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window){
    //按下esc键,关闭窗口
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}


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
    Shader shader("../src/1_2_shader/shader/vert.vs", "../src/1_2_shader/shader/frag.fs");
    
    //三角形1
    float vertices[] = {
        //position  color
        0.25f, 0.25f, 0.0f, 1.0f, 0.0f, 0.0f, //右上角 
        0.0f, -0.25f, 0.0f, 0.0f, 1.0f, 0.0f, //下角
        -0.25f, 0.25f, 0.0f, 0.0f, 0.0f, 1.0f, //左上角
    };
    unsigned int indices[] = {
        0, 1, 2
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); 
    glEnableVertexAttribArray(1);

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

        shader.use();
        double time = glfwGetTime();
        float xOffset = (float)sin(time) * 0.5; 
        shader.setFloat("positionOffsetX", xOffset);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        //检查有没有触发什么事件,比如键盘输入、鼠标移动
        glfwPollEvents();
        //交换缓冲区
        glfwSwapBuffers(window);
    }

    //释放/删除之前分配的所有资源
    glfwTerminate();

    return 0;
}