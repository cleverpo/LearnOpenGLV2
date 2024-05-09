#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#include "../Shader.h"
#include "../models/Shape.h"

const char* DEBUG_NORMAL_VS_SHADER_PATH = "../src/common/shader/debug/debug_normal_vert.vs";
const char* DEBUG_NORMAL_FS_SHADER_PATH = "../src/common/shader/debug/debug_normal_frag.fs";
const char* DEBUG_NORMAL_GS_SHADER_PATH = "../src/common/shader/debug/debug_normal_geo.gs";

const char* DEBUG_TEXTURE_VS_SHADER_PATH = "../src/common/shader/debug/debug_texture_vert.vs";
const char* DEBUG_TEXTURE_FS_SHADER_PATH = "../src/common/shader/debug/debug_texture_frag.fs";

static unsigned int debugTextureQuadVAO = 0;
static Shader* debugTextureShader = NULL;

Shader* createDebugNormalShader(){
    Shader* shader = new Shader(DEBUG_NORMAL_VS_SHADER_PATH, DEBUG_NORMAL_FS_SHADER_PATH, DEBUG_NORMAL_GS_SHADER_PATH);
    return shader;
}

void drawDebugTexture(unsigned int textureId){
    if(!debugTextureShader){
        debugTextureShader = new Shader(DEBUG_TEXTURE_VS_SHADER_PATH, DEBUG_TEXTURE_FS_SHADER_PATH, NULL);
    }
    if(debugTextureQuadVAO == 0){
        float quadVertices[] = {
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };

        glGenVertexArrays(1, &debugTextureQuadVAO);
        unsigned int debugTextureQuadVBO;
        glGenBuffers(1, &debugTextureQuadVBO);
        glBindVertexArray(debugTextureQuadVAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, debugTextureQuadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    debugTextureShader->use();
    debugTextureShader->setInt("tex", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glBindVertexArray(debugTextureQuadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif