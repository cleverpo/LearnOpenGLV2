#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoord;
layout(location = 3) in vec3 aTangent;
layout(location = 4) in vec3 aBitangent;

uniform mat4 model;

out VS_OUT{
    vec3 posWS;
    vec3 normalWS;
    vec2 uv;
}vs_out;

void main()
{   
    gl_Position = model * vec4(aPosition, 1.0);

    vs_out.posWS = (model * vec4(aPosition, 1.0)).xyz;
    vs_out.normalWS = mat3(transpose(inverse(model))) * aNormal;
    vs_out.uv = aTexcoord;
}