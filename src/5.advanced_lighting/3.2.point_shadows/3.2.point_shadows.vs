#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform bool u_ReverseNormals;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoord;
} vs_out;

void main() {
    vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    vs_out.TexCoord = a_TexCoord;

    if(u_ReverseNormals)
        vs_out.Normal = normalize(transpose(inverse(mat3(u_Model))) * (-1.0 * a_Normal));
    else
        vs_out.Normal = normalize(transpose(inverse(mat3(u_Model))) * a_Normal);

    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}