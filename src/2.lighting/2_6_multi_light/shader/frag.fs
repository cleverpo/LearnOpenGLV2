#version 330 core

struct DirectionLight{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirectionLight directionLight;

struct PointLight{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float attenuationFactor1;
    float attenuationFactor2;
    float attenuationFactor3;
};

#define POINT_LIGHTS 4
uniform PointLight pointLights[POINT_LIGHTS];

struct SpotLight{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float cutoff;
    float outerCutoff;
    
    float attenuationFactor1;
    float attenuationFactor2;
    float attenuationFactor3;
};
uniform SpotLight spotLight;

struct Material{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

uniform vec3 cameraPos;

in vec3 posWS;
in vec3 normalWS;
in vec2 uv;

out vec4 FragColor;

vec3 calcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);
    vec3 halfDir = normalize(lightDir + viewDir);
    float nDotL = max(dot(normal, lightDir), 0.0f);
    float nDotH = max(dot(normal, halfDir), 0.0f);
    
    vec3 diffuseColor = texture(material.diffuse, uv).rgb;
    vec3 specularColor = texture(material.specular, uv).rgb;
    
    //环境光照
    vec3 ambient = light.ambient * diffuseColor;
    
    //漫反射
    vec3 diffuse = light.diffuse * diffuseColor * nDotL;

    //镜面发射
    vec3 specular = light.specular * specularColor * pow(nDotH, material.shininess);

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 posWS){
    vec3 lightDir = normalize(light.position - posWS);
    vec3 halfDir = normalize(lightDir + viewDir);

    float nDotL = max(dot(normal, lightDir), 0.0f);
    float nDotH = max(dot(normal, halfDir), 0.0f);

    vec3 diffuseColor = texture(material.diffuse, uv).rgb;
    vec3 specularColor = texture(material.specular, uv).rgb;
    

    //环境光照
    vec3 ambient = light.ambient * diffuseColor;
    
    //漫反射
    vec3 diffuse = light.diffuse * diffuseColor * nDotL;

    //镜面发射
    vec3 specular = light.specular * specularColor * pow(nDotH, material.shininess);

    //着色点与光源距离
    float distance = length(light.position - posWS);
    float attenuation = 1.0 / (light.attenuationFactor1 + light.attenuationFactor2 * distance + light.attenuationFactor3 * (distance * distance));    //衰减公式

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 posWS){
    vec3 lightDir = normalize(light.position - posWS);
    vec3 halfDir = normalize(lightDir + viewDir);

    float nDotL = max(dot(normal, lightDir), 0.0f);
    float nDotH = max(dot(normal, halfDir), 0.0f);

    vec3 diffuseColor = texture(material.diffuse, uv).rgb;
    vec3 specularColor = texture(material.specular, uv).rgb;
    

    //环境光照
    vec3 ambient = light.ambient * diffuseColor;
    
    //漫反射
    vec3 diffuse = light.diffuse * diffuseColor * nDotL;

    //镜面发射
    vec3 specular = light.specular * specularColor * pow(nDotH, material.shininess);

    //用lightDir与light direction的夹角来判断是否在聚光灯内
    float theta = dot(-lightDir, normalize(light.direction));


    //边缘平滑过渡，用内切角与外切角
    float epsilon = light.cutoff - light.outerCutoff;
    //theta小于外切角cos值表示在锥体外面
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
    
    diffuse *= intensity;
    specular *= intensity;
        
    float distance = length(light.position - posWS);
    float attenuation = 1.0 / (light.attenuationFactor1 + light.attenuationFactor2 * distance + light.attenuationFactor3 * (distance * distance));    //衰减公式
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

void main()
{   
    vec3 normal = normalize(normalWS);
    vec3 viewDir = normalize(cameraPos - posWS);

    vec3 finalColor = calcDirectionLight(directionLight, normal, viewDir);

    for(int i = 0; i < POINT_LIGHTS; i++){
        finalColor += calcPointLight(pointLights[i], normal, viewDir, posWS);
    }
    finalColor += calcSpotLight(spotLight, normal, viewDir, posWS);

    FragColor = vec4(finalColor, 1.0f);
}