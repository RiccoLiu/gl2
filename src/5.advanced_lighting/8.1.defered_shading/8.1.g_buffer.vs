
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_out;

void main() {
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));

    fs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    fs_out.Normal = normalize(normalMatrix * a_Normal);
    fs_out.TexCoords = a_TexCoords;
    gl_Position = u_Projection * u_View * vec4(fs_out.FragPos, 1.0);
}
