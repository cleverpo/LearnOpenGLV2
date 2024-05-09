#ifndef TEXTURECUBE_H
#define TEXTURECUBE_H

#include <glad/glad.h>

#include <iostream>
#include <string>
#include <vector>
#include "TextureBase.h"

using namespace std;

struct TextureCubeParameters{
    int wrapS;
    int wrapT;
    int wrapR;
    int minFilter;
    int magFilter;
    int format;
    TextureCubeParameters(int wrapS, int wrapT, int wrapR, int minFilter, int magFilter, int format = -1): 
    wrapS(wrapS), 
    wrapT(wrapT), 
    wrapR(wrapR),
    minFilter(minFilter),
    magFilter(magFilter),
    format(format)
    {};
};

class TextureCube : public TextureBase {
public:
    TextureCube(const vector<string>& pathes, const TextureCubeParameters& params):
    TextureBase()
    {
        //创建纹理
        unsigned int texture;
        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        //设置纹理环绕方式
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, params.wrapS);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, params.wrapT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, params.wrapR);
        //设置纹理过滤方式
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, params.minFilter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, params.magFilter);

        for(unsigned int i = 0; i < pathes.size(); i++){
            //定义宽，高，颜色通道个数
            int width, height, nrChannels;

            stbi_set_flip_vertically_on_load(false);
            
            unsigned char *data = stbi_load(pathes[i].c_str(), &width, &height, &nrChannels, 0);

            if(data){    
                int format = params.format;
                if(format == -1){
                    if (nrChannels == 1)
                        format = GL_RED;
                    else if (nrChannels == 3)
                        format = GL_RGB;
                    else if (nrChannels == 4)
                        format = GL_RGBA;
                }

                //加载并生成纹理
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

                //释放图像内存
                stbi_image_free(data);
            }else{
                std::cout << "TEXTURE_CUBE::FAILED TO LOAD, " << " path: " << pathes[i] << std::endl;
            }
        }
        ID = texture;
    }
    
    void use(int slot){
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_CUBE_MAP, ID);
    }
};
#endif