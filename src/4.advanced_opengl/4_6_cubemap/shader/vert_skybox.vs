#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 view;
uniform mat4 projection;

out vec3 texCoords;

void main()
{   
    texCoords = aPosition;
    vec4 pos = projection * view * vec4(aPosition, 1.0);
    gl_Position =  pos.xyww;
}