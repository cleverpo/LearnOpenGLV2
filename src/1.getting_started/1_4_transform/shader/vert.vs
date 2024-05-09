#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 transform;

out vec4 vColor;
out vec2 vUV;

void main()
{
    gl_Position = transform * vec4(aPosition, 1.0);
    vColor = vec4(aColor, 1.0);
    vUV = aTexCoord;
}