#version 330 core

uniform sampler2D u_DiffuseMap;

in vec2 v_TexCoords;

out vec4 o_FragColor;

void main() {
    o_FragColor = texture(u_DiffuseMap, v_TexCoords);
}
