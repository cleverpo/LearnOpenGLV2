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
uniform int pointLightCount;

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

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D depthMap;

uniform float shininess;

uniform vec3 cameraPos;

in VS_OUT{
    vec3 posWS;
    vec4 posLS;
    vec3 normalWS;
    vec2 uv;
}fs_in;

out vec4 FragColor;

//return 0: out of shadow, 1: in shadow
float calcShadow(vec4 posLS, vec3 normal, vec3 lightDir){
    //light space -> clip space
    vec3 posCS = posLS.xyz / posLS.w;
    //clip space -> ndc space
    vec3 posNDC = posCS.xyz * 0.5 + 0.5;
            
    float currentDepth = posNDC.z;
    vec2 currentUV = posNDC.xy;

    //解决因为深度纹理分辨率低，导致多个像素采样同一个深度产生样条线问题。
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0f;
    //进行pcf，通过多次采样模糊阴影边缘锯齿
    vec2 texelSize = 1.0 / textureSize(depthMap, 0).xy;

    for(int x = -1; x <= 1; x++){
        for(int y = -1; y <= 1; y++){
            float pcfDepth = texture(depthMap, currentUV + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0f;
    
    //超过了光照视锥体的默认没有阴影，因为这样只是产生在光照很远处的地方，没阴影也不太影响效果
    if(posNDC.z > 1.0)
        shadow = 0.0;

    return shadow;
    // return 0.0;
}

vec3 calcDirectionLight(DirectionLight light, vec3 normal, vec3 viewDir){
    vec2 uv = fs_in.uv;
    vec4 posLS = fs_in.posLS;
    vec3 lightDir = normalize(-light.direction);
    vec3 halfDir = normalize(lightDir + viewDir);

    float nDotL = max(dot(normal, lightDir), 0.0f);
    float nDotH = max(dot(normal, halfDir), 0.0f);
    
    vec3 diffuseColor = texture(texture_diffuse1, uv).rgb;
    vec3 specularColor = texture(texture_specular1, uv).rgb;
    
    //环境光照
    vec3 ambient = light.ambient * diffuseColor;
    
    //漫反射
    vec3 diffuse = light.diffuse * diffuseColor * nDotL;

    //镜面发射
    vec3 specular = light.specular * specularColor * pow(nDotH, shininess);

    //阴影
    float shadow = calcShadow(posLS, normal, lightDir);

    return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 viewDir, vec3 posWS){
    vec3 lightDir = normalize(light.position - posWS);
    vec3 halfDir = normalize(lightDir + viewDir);
    vec2 uv = fs_in.uv;

    float nDotL = max(dot(normal, lightDir), 0.0f);
    float nDotH = max(dot(normal, halfDir), 0.0f);

    vec3 diffuseColor = texture(texture_diffuse1, uv).rgb;
    vec3 specularColor = texture(texture_specular1, uv).rgb;
    

    //环境光照
    vec3 ambient = light.ambient * diffuseColor;
    
    //漫反射
    vec3 diffuse = light.diffuse * diffuseColor * nDotL;

    //镜面发射
    vec3 specular = light.specular * specularColor * pow(nDotH, shininess);

    //着色点与光源距离
    float distance = length(light.position - posWS);
    float attenuation = 1.0 / (light.attenuationFactor1 + light.attenuationFactor2 * distance + light.attenuationFactor3 * (distance * distance));    //衰减公式

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 viewDir, vec3 posWS){
    vec3 lightDir = normalize(light.position - posWS);
    vec3 halfDir = normalize(lightDir + viewDir);
    vec2 uv = fs_in.uv;

    float nDotL = max(dot(normal, lightDir), 0.0f);
    float nDotH = max(dot(normal, halfDir), 0.0f);

    vec3 diffuseColor = texture(texture_diffuse1, uv).rgb;
    vec3 specularColor = texture(texture_specular1, uv).rgb;
    

    //环境光照
    vec3 ambient = light.ambient * diffuseColor;
    
    //漫反射
    vec3 diffuse = light.diffuse * diffuseColor * nDotL;

    //镜面发射
    vec3 specular = light.specular * specularColor * pow(nDotH, shininess);

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

vec3 gamma(vec3 color){
    float gamma = 2.2;
    return pow(color, vec3(1.0 / gamma));
}

void main()
{   
    vec3 normal = normalize(fs_in.normalWS);
    vec3 viewDir = normalize(cameraPos - fs_in.posWS);

    vec3 finalColor = calcDirectionLight(directionLight, normal, viewDir);

    for(int i = 0; i < pointLightCount; i++){
        finalColor += calcPointLight(pointLights[i], normal, viewDir, fs_in.posWS);
    }

    // finalColor = gamma(finalColor);

    FragColor = vec4(finalColor, 1.0f);
}