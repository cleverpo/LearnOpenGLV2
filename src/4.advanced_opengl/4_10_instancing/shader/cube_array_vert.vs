#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform vec2 offsets[100];

void main()
{   
    vec2 offset = offsets[gl_InstanceID];
    gl_Position = vec4(0.1 * aPosition.xy + offset, 0.0, 1.0);
}