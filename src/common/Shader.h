#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <iostream>

#include "FileUtils.h"

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexFilePath, const char* fragmentFilePath, const char* geometryFilePath = nullptr){
        FileUtils* fileUtils = FileUtils::getInstance();

        std::string vertexCode = fileUtils->readFile(vertexFilePath);
        std::string fragmentCode = fileUtils->readFile(fragmentFilePath);
        std::string geometryCode = fileUtils->readFile(geometryFilePath);
        
        //编译着色器
        unsigned int vertexShader, fragmentShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        glShaderSource(vertexShader, 1, &vShaderCode, NULL);
        glCompileShader(vertexShader);
        checkCompileShader(vertexShader, "VERTEX");
        
        glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
        glCompileShader(fragmentShader);
        checkCompileShader(fragmentShader, "FRAGMENT");
        
        //创建着色器程序
        unsigned int shaderProgram;
        shaderProgram = glCreateProgram();
        //把着色器附加到程序对象上
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        //几何着色器
        unsigned int geometryShader;
        if(geometryFilePath != nullptr){
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

            const char* gShaderCode = geometryCode.c_str();

            glShaderSource(geometryShader, 1, &gShaderCode, NULL);
            glCompileShader(geometryShader);
            checkCompileShader(geometryShader, "GEOMETRY");

            glAttachShader(shaderProgram, geometryShader);
        }

        //链接着色器程序
        glLinkProgram(shaderProgram);
        checkLinkProgram(shaderProgram);
        
        //把着色器链接到着色器程序后，着色器对象就不需要用到了，可以删除
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if(geometryFilePath != nullptr){
            glDeleteShader(geometryShader);
        }

        ID = shaderProgram;
    };

    void use(){
        glUseProgram(ID);
    };

    void setBool(const std::string &name, bool value){
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    };

    void setInt(const std::string &name, int value){
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    };

    void setFloat(const std::string &name, float value){
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    };

    void setVec2(const std::string &name, const float* v){
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, v);
    };

    void setVec3(const std::string &name, const float* v){
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, v);
    };

    void setMat4(const std::string &name, const float* value){
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
    };

private:
    void checkCompileShader(unsigned int shaderId, const char* shaderType){
        int success;
        char infoLog[512];
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if(!success){
            glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    }

    void checkLinkProgram(unsigned int programId){
        //检查链接是否成功
        int success;
        char infoLog[512];
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if(!success){
            glGetProgramInfoLog(programId, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
        }
    }
};

#endif