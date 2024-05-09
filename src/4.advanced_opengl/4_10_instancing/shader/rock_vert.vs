#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;
layout(location = 3) in mat4 aInstanceMatrix;

uniform mat4 projection;
uniform mat4 view;

out vec2 uv;

void main()
{
    gl_Position = projection * view * aInstanceMatrix * vec4(aPosition, 1.0); 
    uv = aTexcoord;
}