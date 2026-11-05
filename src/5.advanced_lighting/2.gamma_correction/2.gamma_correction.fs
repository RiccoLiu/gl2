#version 330 core

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light u_Light;
uniform vec3 u_ViewPos;
uniform sampler2D u_Texture;

uniform bool gamma;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} fs_in;

out vec4 o_FragColor;

void main() {
    vec3 color = texture(u_Texture, fs_in.TexCoord).rgb;

    vec3 ambient = u_Light.ambient * color;

    vec3 lightDir = normalize(u_Light.position - fs_in.FragPos);
    vec3 diff = max(dot(fs_in.Normal, lightDir), 0.0) * color;
    vec3 diffuse = diff * u_Light.diffuse;

    vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(fs_in.Normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * u_Light.specular;

    // 衰减
    float distance = length(u_Light.position - fs_in.FragPos);
    float attenuation = 1.0 / (gamma ? distance * distance : distance);
    diffuse *= attenuation;
    specular *= attenuation;

    color = ambient + diffuse + specular;

    if(gamma)
        color = pow(color, vec3(1.0/2.2));

    o_FragColor = vec4(color, 1.0);
}
