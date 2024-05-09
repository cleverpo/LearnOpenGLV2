#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 5) out;

in VS_OUT{
    vec3 posWS;
    vec3 normalWS;
    vec2 uv;
} gs_in[];

uniform mat4 view;
uniform mat4 projection;

uniform float time;

out vec3 posWS;
out vec3 normalWS;
out vec2 uv;

vec3 getNormal(){
    vec3 a = (gl_in[0].gl_Position - gl_in[1].gl_Position).xyz;
    vec3 b = (gl_in[2].gl_Position - gl_in[1].gl_Position).xyz;
    return normalize(cross(a, b));
};

void explode(int index, vec3 normal){
    posWS = gs_in[index].posWS;
    normalWS = gs_in[index].normalWS;
    uv = gs_in[index].uv;

    vec4 position = gl_in[index].gl_Position;
    float explodeRange = 0.2;
    vec3 offset = normal * explodeRange * (sin(time) * 0.5 + 0.5);
    
    gl_Position = projection * view * vec4(vec3(position) + offset, 1.0);

    EmitVertex();
};

void main()
{   
    vec3 normal = getNormal();
    explode(0, normal);
    explode(1, normal);
    explode(2, normal);

    EndPrimitive();
}