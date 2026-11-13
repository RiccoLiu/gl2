#version 330 core

uniform sampler2D u_PositionMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_AlbedoSpecMap;

struct Light {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
    
    float Linear;
    float Quadratic;
    float Radius;
};
const int NR_LIGHTS = 32;

uniform Light u_Lights[NR_LIGHTS];
uniform vec3 u_ViewPos;

in vec2 v_TexCoords;

out vec4 o_FragColor;

vec3 BlinnPhong() {
    vec3 fragPos = texture(u_PositionMap, v_TexCoords).rgb;
    vec3 normal = texture(u_NormalMap, v_TexCoords).rgb;
    vec3 color = texture(u_AlbedoSpecMap, v_TexCoords).rgb;
    float s = texture(u_AlbedoSpecMap, v_TexCoords).a; 

    vec3 blinnPhong;
    for (int i = 0; i < NR_LIGHTS; i++) {
        float distance = length(u_Lights[i].Position - fragPos);
        if (distance < u_Lights[i].Radius) {
            vec3 ambient = u_Lights[i].Ambient * color;
            
            vec3 lightDir = normalize(u_Lights[i].Position - fragPos);
            float diff = max(dot(lightDir, normal), 0.0);
            vec3 diffuse = u_Lights[i].Diffuse * diff * color;

            vec3 viewDir = normalize(u_ViewPos - fragPos);
            vec3 halfDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(normal, halfDir), 0.0), 16);
            vec3 specular = u_Lights[i].Specular * spec * s;

            float attenuation = 1.0 / (1.0 + u_Lights[i].Linear * distance + u_Lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;   

            vec3 light = ambient + diffuse + specular;    
            blinnPhong += light;
        }
    }
    return blinnPhong;
}

void main() {
    o_FragColor = vec4(BlinnPhong(), 1.0);
}