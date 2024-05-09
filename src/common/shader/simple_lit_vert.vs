#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 posWS;
out vec3 normalWS;
out vec2 uv;

void main()
{   
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    posWS = (model * vec4(aPosition, 1.0)).xyz;
    normalWS = mat3(transpose(inverse(model))) * aNormal;

    uv = aTexcoord;
}