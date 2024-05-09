#version 330 core

uniform vec3 lightColor;

void main() {
    vec3 color = gl_FrontFacing ? lightColor : vec3(0.0f);
    gl_FragColor = vec4(color, 1.0f);
}
