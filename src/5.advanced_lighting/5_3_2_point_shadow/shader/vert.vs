#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexcoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out VS_OUT{
    vec3 posWS;
    vec4 posLS;
    vec3 normalWS;
    vec2 uv;
}vs_out;

void main()
{   
    vec4 pos = vec4(aPosition, 1.0);
    vec4 posWS = model * pos;

    gl_Position = projection * view * posWS;

    vs_out.posWS = posWS.xyz;
    vs_out.posLS = lightSpaceMatrix * posWS;
    vs_out.normalWS = mat3(transpose(inverse(model))) * aNormal;
    vs_out.uv = aTexcoords;
}