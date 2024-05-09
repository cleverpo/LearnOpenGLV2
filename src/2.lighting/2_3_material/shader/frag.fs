#version 330 core

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
uniform Material material;

uniform vec3 cameraPos;

in vec3 posWS;
in vec3 normalWS;

out vec4 FragColor;

void main()
{   
    vec3 normal = normalize(normalWS);
    vec3 lightDir = normalize(light.position - posWS);
    vec3 viewDir = normalize(cameraPos - posWS);
    vec3 halfDir = normalize(lightDir + viewDir);

    float nDotL = max(dot(normal, lightDir), 0.0f);
    float nDotH = max(dot(normal, halfDir), 0.0f);

    //环境光照
    vec3 ambient = light.ambient * material.ambient;
    
    //漫反射
    vec3 diffuse = light.diffuse * material.diffuse * nDotL;

    //镜面发射
    vec3 specular = light.specular * material.specular * pow(nDotH, material.shininess);

    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(finalColor, 1.0f);
}