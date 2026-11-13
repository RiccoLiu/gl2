#version 330 core

layout (location = 0) out vec4 o_FragColor;
layout (location = 1) out vec4 o_BloomColor;

uniform sampler2D u_Texture;

struct Light {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

uniform Light u_Lights[4];
uniform vec3 u_ViewPos;
uniform bool u_RenderLight;

uniform vec3 u_LightColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

vec3 BlinnPhong() {
    vec3 color = texture(u_Texture, fs_in.TexCoords).rgb;

    vec3 lightAll;
    for (int i = 0; i < 4; i++) {
        vec3 ambient = u_Lights[i].Ambient * color;
        
        vec3 lightDir = normalize(u_Lights[i].Position - fs_in.FragPos);
        float diff = max(dot(lightDir, fs_in.Normal), 0.0);
        vec3 diffuse = diff * u_Lights[i].Diffuse * color;

        vec3 viewDir = normalize(u_ViewPos - fs_in.FragPos);
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(halfDir, fs_in.Normal), 0), 32);
        vec3 specular = u_Lights[i].Specular * spec;

        vec3 light = ambient + diffuse + specular;

        float distance = length(fs_in.FragPos - u_Lights[i].Position);
        light *= 1.0 / (distance * distance);
        lightAll = lightAll + light;
    }    
    return lightAll;
}

void main() {
    vec3 light;

    if (u_RenderLight) {
        light = u_LightColor;
    } else {
        light = BlinnPhong();
    }

    o_FragColor = vec4(light, 1.0);

    float brightness = dot(o_FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        o_BloomColor = vec4(o_FragColor.rgb, 1.0);
    } else {
        o_BloomColor = vec4(vec3(0.0), 1.0);
    }
}

