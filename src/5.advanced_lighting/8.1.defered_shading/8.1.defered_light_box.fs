#version 330 core

uniform vec3 u_LightColor;

out vec4 o_FragColor;

void main() {
    o_FragColor = vec4(u_LightColor, 1.0);
}
