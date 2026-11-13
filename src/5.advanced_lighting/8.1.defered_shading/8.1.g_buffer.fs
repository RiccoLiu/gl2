#version 330 core

layout (location = 0) out vec3 g_Position;
layout (location = 1) out vec3 g_Normal;
layout (location = 2) out vec4 g_AlbedoSpec;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

void main() {
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    float spec = texture(texture_specular1, fs_in.TexCoords).r;

    g_Position = fs_in.FragPos;
    g_Normal = fs_in.Normal;
    g_AlbedoSpec.rgb = color;
    g_AlbedoSpec.a = spec;
}

