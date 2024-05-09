#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vUV;

void main()
{
    mat4 transform = projection * view * model;
    gl_Position = transform * vec4(aPosition, 1.0);
    vUV = aTexCoord;
}