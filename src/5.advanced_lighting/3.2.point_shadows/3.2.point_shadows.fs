
#version 330 core

uniform sampler2D u_WoodTexture;
uniform samplerCube u_DepthMap;

uniform vec3 u_ViewPos;
uniform float u_FarPlane;

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
    vec2 TexCoord;
} fs_in;

out vec4 o_FragColor;

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);


float GetShadow(vec3 fragPos) {
    vec3 fragDir = fragPos - u_Light.position;

    float closestDepth = texture(u_DepthMap, fragDir).r;
    closestDepth *= u_FarPlane;

    float currentDepth = length(fragDir);

    float shadow = 0.0;

    // 1. 阴影偏移优化阴影失真
    // float bias = 0.05; // 0.05;
    // shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    // 2. PCF 优化阴影边缘锯齿
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(u_ViewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / u_FarPlane)) / 75.0; // 距离越远阴影越柔和，距离越近阴影越锐利
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(u_DepthMap, fragDir + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= u_FarPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

vec3 BlinphPhong() {
    vec3 color = texture(u_WoodTexture, fs_in.TexCoord).rgb;

    vec3 ambient = u_Light.ambient;

    vec3 lightDir = normalize(u_Light.position - fs_in.FragPos);
    float diff = max(dot(lightDir, fs_in.Normal), 0);
    vec3 diffuse = u_Light.diffuse * diff;

    vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
    vec3 halfward = normalize(viewDir + lightDir);
    float spec = pow(max(dot(halfward, fs_in.Normal), 0), 64);
    vec3 specula = u_Light.specular * spec;

    float shadow = GetShadow(fs_in.FragPos);
    return (ambient + (1.0 - shadow)*(diffuse + specula)) * color;
}

void main() {
    // 显示深度图
    // vec3 fragDir = fs_in.FragPos - u_Light.position;
    // float closestDepth = texture(u_DepthMap, fragDir).r;
    // o_FragColor = vec4(vec3(closestDepth), 1.0);

    o_FragColor = vec4(BlinphPhong(), 1.0);
}

