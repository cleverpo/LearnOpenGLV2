#ifndef MATERIAL_H
#define MATERIAL_H

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Texture2D.h"

struct Material
{
    Texture2D* diffuse;
    Texture2D* specular;
    Texture2D* emission;
    float shininess;
};

#endif