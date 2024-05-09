#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;

out vec3 posVS;
out vec3 normalVS;
out vec3 lightPosVS;

void main()
{   
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    posVS = (view * model * vec4(aPosition, 1.0)).xyz;
    //法线用法线矩阵,如果用model矩阵,会因为不等比缩放导致法线不垂直问题.
    //法线矩阵是模型矩阵的逆矩阵的转置矩阵
    normalVS = mat3(transpose(inverse(view * model))) * aNormal;

    lightPosVS = (view * vec4(lightPos, 1.0)).xyz;
}