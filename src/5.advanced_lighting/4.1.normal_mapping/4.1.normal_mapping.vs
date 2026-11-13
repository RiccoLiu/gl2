
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform vec3 u_ViewPos;
uniform vec3 u_LightPos;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentFragPos;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
} vs_out;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    
    vec3 T = normalize(normalMatrix * a_Tangent);
    vec3 N = normalize(normalMatrix * a_Normal);

    T = normalize(T - dot(T, N) * N);
    vec3 B = normalize(cross(N, T));

    mat3 invTBN = transpose(mat3(T, B, N));

    vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    vs_out.TexCoord = a_TexCoord;
    vs_out.TangentFragPos = invTBN * vs_out.FragPos;
    vs_out.TangentLightPos = invTBN * u_LightPos;
    vs_out.TangentViewPos = invTBN * u_ViewPos;

    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}

