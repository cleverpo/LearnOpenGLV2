#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT{
    vec3 normalWS;
}gs_in[];

uniform mat4 view;
uniform mat4 projection;

void drawNormal(vec4 position, vec3 normal){
    gl_Position = projection * view * position;
    EmitVertex();

    float lineLength = 0.05f;
    gl_Position = projection * view * (position + vec4(normal * lineLength, 0.0));
    EmitVertex();

    EndPrimitive();
};

void main()
{   
    drawNormal(gl_in[0].gl_Position, gs_in[0].normalWS);
    drawNormal(gl_in[1].gl_Position, gs_in[1].normalWS);
    drawNormal(gl_in[2].gl_Position, gs_in[2].normalWS);
}