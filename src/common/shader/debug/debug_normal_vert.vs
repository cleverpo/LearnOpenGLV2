#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;

out VS_OUT{
    vec3 normalWS;
}vs_out;

void main()
{   
    gl_Position = model * vec4(aPosition, 1.0);
    vs_out.normalWS = mat3(transpose(inverse(model))) * aNormal;
}