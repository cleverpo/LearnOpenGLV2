#ifndef TEXTUREBASE_H
#define TEXTUREBASE_H

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>


class TextureBase{
public:
    unsigned int ID;

    TextureBase():
    ID(0)
    {
    }
};
#endif