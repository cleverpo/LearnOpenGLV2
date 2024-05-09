#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window){
    //按下esc键,关闭窗口
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

unsigned int createVAO(const float* vertices, int verticesSize, const unsigned int* indices, int indicesSize){
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
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertices, GL_STATIC_DRAW);
    
    //创建索引缓冲对象
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    //绑定索引缓冲对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //把索引数据复制到缓冲对象
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indices, GL_STATIC_DRAW);

    //设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //第一个参数0，代表把之前绑定的VBO的顶点数据传递到顶点着色器的位置0
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    return VAO;
}

unsigned int createShaderProgram(const char* vs, const char* fs){
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vs, NULL);
    glCompileShader(vertexShader);
    //检查编译是否成功
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fs, NULL);
    glCompileShader(fragmentShader);
    //检查编译是否成功
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    //创建着色器程序
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    //把着色器附加到程序对象上
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //链接着色器程序
    glLinkProgram(shaderProgram);
    //检查链接是否成功
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
        return 0;
    }
    //把着色器链接到着色器程序后，着色器对象就不需要用到了，可以删除
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
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
    //注册窗口大小改变的回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //初始化GLAD
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //顶点着色器
    const char* vertexShaderSource = 
    "#version 330 core \n"
    "layout (location = 0) in vec3 aPos; \n"
    "void main() \n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
    
    //片段着色器
    const char* fragmentShaderSource1 = 
    "#version 330 core \n"
    "out vec4 FragColor; \n"
    "void main() \n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
    "}\0";
    
    //片段着色器
    const char* fragmentShaderSource2 = 
    "#version 330 core \n"
    "out vec4 FragColor; \n"
    "void main() \n"
    "{\n"
    "   FragColor = vec4(0.0f, 10.0f, 0.0f, 1.0f);\n"
    "}\0";
    int shaderProgram1 = createShaderProgram(vertexShaderSource, fragmentShaderSource1);
    if(shaderProgram1 == -1){
        return -1;
    }
    int shaderProgram2 = createShaderProgram(vertexShaderSource, fragmentShaderSource2);
    if(shaderProgram2 == -1){
        return -1;
    }
    
    //创建顶点数组对象
    
    //三角形1
    float vertices1[] = {
        0.0f, 0.25f, 0.0f, //右上角
        -0.25f, -0.25f, 0.0f, //下角
        -0.5f, 0.25f, 0.0f, //左上角
    };
    unsigned int indices1[] = {
        0, 1, 2, //第一个三角形
    };
    
    //三角形2
    float vertices2[] = {
        0.5f, 0.25f, 0.0f, //右上角
        0.25f, -0.25f, 0.0f, //下角
        0.0f, 0.25f, 0.0f, //左上角
    };
    unsigned int indices2[] = {
        0, 1, 2, //第一个三角形
    };

    unsigned int VAO1 = createVAO(vertices1, sizeof(vertices1), indices1, sizeof(indices1));
    unsigned int VAO2 = createVAO(vertices2, sizeof(vertices2), indices2, sizeof(indices2));

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

        glUseProgram(shaderProgram1);
        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO2);
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