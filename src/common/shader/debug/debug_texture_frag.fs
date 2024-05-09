#version 330 core

uniform sampler2D tex;

in VS_OUT{
    vec2 uv;
}fs_in;

out vec4 FragColor;

void main()
{   
    FragColor = texture(tex, fs_in.uv);
}