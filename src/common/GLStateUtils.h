#ifndef GL_STATE_UTILS_H
#define GL_STATE_UTILS_H

#include <glad/glad.h>

class GLStateUtils{
public:
    static GLStateUtils* getInstance();

    bool isCullFace();
    GLenum getCullFaceMode();
private:
    GLStateUtils(){}

    static GLStateUtils* _instance;
};

GLStateUtils* GLStateUtils::_instance = nullptr;
GLStateUtils* GLStateUtils::getInstance(){
    if(GLStateUtils::_instance == nullptr){
        GLStateUtils::_instance = new GLStateUtils();
    }
    return GLStateUtils::_instance;
};

bool GLStateUtils::isCullFace(){
    GLboolean oldState;
    glGetBooleanv(GL_CULL_FACE, &oldState);
    
    return oldState == GL_TRUE;
}

GLenum GLStateUtils::getCullFaceMode(){
    GLint mode;
    glGetIntegerv(GL_CULL_FACE_MODE, &mode);
    return static_cast<GLenum>(mode);
}
#endif