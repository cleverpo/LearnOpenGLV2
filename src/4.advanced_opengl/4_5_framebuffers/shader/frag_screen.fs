#version 330 core

in vec2 uv0;
in vec2 uv1;
in vec2 uv2;
in vec2 uv3;
in vec2 uv4;
in vec2 uv5;
in vec2 uv6;
in vec2 uv7;
in vec2 uv8;

uniform sampler2D sreenTexture;

out vec4 FragColor;

vec4 opposition(vec4 color){
    return 1.0 - color;
}

float grayscale(vec3 color){
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

vec3 gaussianBlur(){
    float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );
    vec3 color = vec3(0.0);
    color += texture(sreenTexture, uv0).rgb * kernel[0];
    color += texture(sreenTexture, uv1).rgb * kernel[1];
    color += texture(sreenTexture, uv2).rgb * kernel[2];
    color += texture(sreenTexture, uv3).rgb * kernel[3];
    color += texture(sreenTexture, uv4).rgb * kernel[4];
    color += texture(sreenTexture, uv5).rgb * kernel[5];
    color += texture(sreenTexture, uv6).rgb * kernel[6];
    color += texture(sreenTexture, uv7).rgb * kernel[7];
    color += texture(sreenTexture, uv8).rgb * kernel[8];

    return color;
}

void main()
{   
    // FragColor = vec4(opposition(texture(sreenTexture, uv4)));

    // FragColor = vec4(vec3(grayscale(texture(sreenTexture, uv4).rgb)), 1.0);

    FragColor = vec4(gaussianBlur(), 1.0);
}