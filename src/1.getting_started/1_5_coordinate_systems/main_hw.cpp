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

float cameraX = 0.0f;
float cameraY = 0.0f;
float cameraZ = 3.0f;
glm::mat4 view = glm::mat4(1.0f);

float fov = 90.0f;
float aspect = 800.0f/600.0f;
glm::mat4 projection = glm::perspective(glm::radians(fov * 0.5f), aspect, 0.1f, 100.0f);

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
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    //注册窗口大小改变的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //着色器
    Shader shader("../src/1_5_coordinate_systems/shader/vert.vs", "../src/1_5_coordinate_systems/shader/frag.fs");

    //纹理
    Texture2D texture1("../texture/container.jpg", Texture2DParameters(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR, GL_RGB, true));
    Texture2D texture2("../texture/awesomeface.png", Texture2DParameters(GL_MIRRORED_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR, GL_RGBA,  true));
    
    shader.use();
    shader.setInt("texture0", 0);
    shader.setInt("texture1", 1);

    //立方体
    // float vertices[] = {
    //     0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    //     0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    //     -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
    //     0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
    // };
    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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

    //设置顶点属性指针
    /**
     * 位置属性(对应shader里location)
     * 数量
     * 是否normalize
     * 步长
     * 偏移
     **/
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); 
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    //线框模式
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //普通模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //变换矩阵
    //模型矩阵
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //观察矩阵
    view = glm::translate(view, glm::vec3(cameraX, cameraY, -cameraZ));

    //投影矩阵
    //透视投影
    /**
     * 视角（FOV/2)
     * 宽高比(宽/高)
     * 近平面
     * 远平面
    */
    projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);
    //正交投影
    /**
     * 左
     * 右
     * 下
     * 上
     * 近
     * 远
    */
    // projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);

    //渲染循环
    while(!glfwWindowShouldClose(window)){
        //获取用户交互输入
        processInput(window);

        //渲染指令
        //清空屏幕
        glClearColor(0.2f, 0.1f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texture1.use(0);
        texture2.use(1);
        

        shader.use();
        model = glm::mat4(1.0f);
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        shader.setMat4("model", glm::value_ptr(model));
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        glBindVertexArray(VAO);

        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -1.0f));
        model = glm::rotate(model, (float)glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", glm::value_ptr(model));

        glDrawArrays(GL_TRIANGLES, 0, 36);

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
    
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        fov += 1.0f;
        projection = glm::perspective(glm::radians(fov * 0.5f), aspect, 0.1f, 100.0f);
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        fov -= 1.0f;
        projection = glm::perspective(glm::radians(fov * 0.5f), aspect, 0.1f, 100.0f);
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        aspect -= 0.01f;
        projection = glm::perspective(glm::radians(fov * 0.5f), aspect, 0.1f, 100.0f);
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        aspect += 0.01f;
        projection = glm::perspective(glm::radians(fov * 0.5f), aspect, 0.1f, 100.0f);
    }
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        cameraZ -= 0.01f;
        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(cameraX, cameraY, -cameraZ));
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        cameraZ += 0.01f;
        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(cameraX, cameraY, -cameraZ));
    }
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        cameraX -= 0.01f;
        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(cameraX, cameraY, -cameraZ));
    }
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        cameraX += 0.01f;
        view = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(cameraX, cameraY, -cameraZ));
    }
}