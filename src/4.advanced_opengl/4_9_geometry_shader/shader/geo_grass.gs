#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

out vec3 fColor;

vec3 getNormal(){
    vec3 a = (gl_in[0].gl_Position - gl_in[1].gl_Position).xyz;
    vec3 b = (gl_in[2].gl_Position - gl_in[1].gl_Position).xyz;
    return normalize(cross(a, b));
};

void drawGrass(vec4 position, vec3 normal){
    fColor = vec3(1.0, 1.0, 1.0);
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下
    EmitVertex();   
    gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:右下
    EmitVertex();
    
    fColor = vec3(0.0, 0.5, 0.0);
    gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:左上
    EmitVertex();
    gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:右上
    EmitVertex();
    
    fColor = vec3(0.0, 1.0, 0.0);
    gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:顶部
    EmitVertex();
    EndPrimitive();
};

void main()
{   
    vec3 normal = getNormal();

    drawGrass(gl_in[0].gl_Position, normal);
    drawGrass(gl_in[1].gl_Position, normal);
    drawGrass(gl_in[2].gl_Position, normal);
}