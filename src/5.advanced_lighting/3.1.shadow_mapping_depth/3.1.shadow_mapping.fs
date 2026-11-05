
#version 330 core

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_ShadowMap;

uniform vec3 u_ViewPos;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light u_Light;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 o_FragColor;

float GetShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) { 
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(u_ShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // 超过正交视锥的远平面
    if (projCoords.z > 1.0) {
        return 0.0;
    }

    // 阴影偏移优化阴影失真
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}

vec3 BlinphPhong() { 
    vec3 color = texture(u_DiffuseMap, fs_in.TexCoords).rgb;
    
    vec3 ambient = u_Light.ambient;
    
    vec3 lightDir = normalize(u_Light.position - fs_in.FragPos);
    float diff = max(dot(lightDir, fs_in.Normal), 0);
    vec3 diffuse = u_Light.diffuse * diff;

    vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
    vec3 halfward = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfward, fs_in.Normal), 0), 64);
    vec3 specula = u_Light.specular * spec;

    float shadow = GetShadow(fs_in.FragPosLightSpace, fs_in.Normal, lightDir);
    // float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    return (ambient + (1.0 - shadow) * (diffuse + specula)) * color;
}

void main() {    
    o_FragColor = vec4(BlinphPhong(), 1.0);
}