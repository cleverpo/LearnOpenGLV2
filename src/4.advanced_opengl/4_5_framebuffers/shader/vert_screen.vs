#version 330 core

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexcoord;

uniform vec2 texelSize;

out vec2 uv0;
out vec2 uv1;
out vec2 uv2;
out vec2 uv3;
out vec2 uv4;
out vec2 uv5;
out vec2 uv6;
out vec2 uv7;
out vec2 uv8;

void main()
{   
    gl_Position = vec4(aPosition.x, aPosition.y, 0.0, 1.0);
    float scale = 4.0f;
    uv0 = aTexcoord + vec2(-texelSize.x,    texelSize.y) * scale;
    uv1 = aTexcoord + vec2(0,               texelSize.y) * scale;
    uv2 = aTexcoord + vec2(texelSize.x,     texelSize.y) * scale;
    uv3 = aTexcoord + vec2(-texelSize.x,        0) * scale;
    uv4 = aTexcoord + vec2(0,                   0) * scale;
    uv5 = aTexcoord + vec2(texelSize.x,         0) * scale;
    uv6 = aTexcoord + vec2(-texelSize.x,    -texelSize.y) * scale;
    uv7 = aTexcoord + vec2(0,               -texelSize.y) * scale;
    uv8 = aTexcoord + vec2(texelSize.x,     -texelSize.y) * scale;
}