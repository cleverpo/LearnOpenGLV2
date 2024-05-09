#version 330 core

in vec4 vColor;
in vec2 vUV;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float mixValue;

out vec4 FragColor;

void main()
{
    vec4 color0 = texture(texture0, vUV);
    vec4 color1 = texture(texture1, vUV);
    FragColor = mix(color0, color1, mixValue);
}