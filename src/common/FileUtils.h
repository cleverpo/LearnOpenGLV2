#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class FileUtils{
public:
    static FileUtils* getInstance();

    std::string readFile(const char* filePath);
private:
    FileUtils(){}

    static FileUtils* _instance;
};

FileUtils* FileUtils::_instance = nullptr;
FileUtils* FileUtils::getInstance(){
    if(FileUtils::_instance == nullptr){
        FileUtils::_instance = new FileUtils();
    }
    return FileUtils::_instance;
};

std::string FileUtils::readFile(const char* filePath){
    if(!filePath) return std::string();
    
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    //读取文件
    try{
        file.open(filePath);

        std::stringstream filestream;
        filestream << file.rdbuf();

        file.close();

        return filestream.str();
    }catch(std::ifstream::failure e){
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << filePath << std::endl;
        return std::string();
    }
};

#endif