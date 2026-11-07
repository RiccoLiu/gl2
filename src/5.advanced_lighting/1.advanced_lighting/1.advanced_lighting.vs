#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat3 u_NormalMatrix;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

void main() {
    vs_out.FragPos = a_Position;
    vs_out.Normal = normalize(u_NormalMatrix * a_Normal);
    vs_out.TexCoords = a_TexCoords;
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}