#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <glad/glad.h>

#include <iostream>
#include "TextureBase.h"
#include <filesystem.h>

struct Texture2DParameters{
    int wrapS;
    int wrapT;
    int minFilter;
    int magFilter;
    int format;
    bool flipVertically;
    bool gamma;
    Texture2DParameters(int wrapS, int wrapT, int minFilter, int magFilter, int format = -1, bool flipVertically = true, bool gamma = false): 
    wrapS(wrapS), 
    wrapT(wrapT), 
    minFilter(minFilter),
    magFilter(magFilter),
    format(format),
    flipVertically(flipVertically),
    gamma(gamma)
    {};
};

class Texture2D : public TextureBase {
public:
    Texture2D(const char* path, const Texture2DParameters& params):
    TextureBase()
    {
        //定义宽，高，颜色通道个数
        int width, height, nrChannels;

        stbi_set_flip_vertically_on_load(params.flipVertically);
        
        unsigned char *data = stbi_load(FileSystem::getPath(path).c_str(), &width, &height, &nrChannels, 0);

        if(data){    
            //创建纹理
            unsigned int texture;
            glGenTextures(1, &texture);

            glBindTexture(GL_TEXTURE_2D, texture);
            //设置纹理环绕方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapT);
            //设置纹理过滤方式
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.magFilter);

            int format = params.format;
            int internalformat = format;
            bool gamma = params.gamma;
            if(format == -1){
                if (nrChannels == 1){
                    format = internalformat = GL_RED;
                }else if (nrChannels == 3){
                    format = internalformat = GL_RGB;
                    if(gamma)
                        internalformat = GL_SRGB;
                }else if (nrChannels == 4){
                    format = internalformat = GL_RGBA;
                    if(gamma)
                        internalformat = GL_SRGB_ALPHA;
                }
            }

            //加载并生成纹理
            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            //生成mipmap
            glGenerateMipmap(GL_TEXTURE_2D);

            ID = texture;
            std::cout << "TEXTURE2D::SUCCESS LOADED" << " path: " << path << std::endl;
        }else{
            std::cout << "TEXTURE2D::FAILED TO LOAD, " << " path: " << path << std::endl;
        }

        //释放图像内存
        stbi_image_free(data);
        stbi_set_flip_vertically_on_load(false);
    }
    
    void use(int slot){
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, ID);
    }
};
#endif