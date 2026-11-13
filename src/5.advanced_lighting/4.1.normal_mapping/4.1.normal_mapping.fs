#version 330 core

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_NormalMap;

struct Light {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};
uniform Light u_Light;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentFragPos;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
} fs_in;

out vec4 o_FragColor;

vec3 BlinnPhong() {
    vec3 normal = normalize(texture(u_NormalMap, fs_in.TexCoord).rgb);
    normal = normalize(normal * 2.0 - 1.0);  // normalize [-1, 1]

    vec3 color = texture(u_DiffuseMap, fs_in.TexCoord).rgb;

    vec3 ambient = u_Light.Ambient * color;

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffuse = u_Light.Diffuse * diff * color;
    
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfVector = normalize(viewDir + lightDir);

    float spec = pow(max(dot(halfVector, normal), 0), 32);
    vec3 specular = u_Light.Specular * spec;

    return ambient + diffuse + specular;
}

void main() {
    // o_FragColor = texture(u_DiffuseMap, fs_in.TexCoord);
    o_FragColor = vec4(BlinnPhong(), 1.0);
}
