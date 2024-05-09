#version 330 core

uniform samplerCube skyboxTexture;

in vec3 texCoords;

out vec4 FragColor;

void main()
{   
    FragColor = texture(skyboxTexture, texCoords);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}