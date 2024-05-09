#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 posWS;
out vec3 normalWS;

void main()
{   
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    posWS = (model * vec4(aPosition, 1.0)).xyz;
    //法线用法线矩阵,如果用model矩阵,会因为不等比缩放导致法线不垂直问题.
    //法线矩阵是模型矩阵的逆矩阵的转置矩阵
    normalWS = mat3(transpose(inverse(model))) * aNormal;
}