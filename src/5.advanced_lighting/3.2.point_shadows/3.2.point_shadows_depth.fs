
#version 330 core

uniform vec3 u_LightPos;
uniform float u_FarPlane;

in vec4 g_FragPos;

void main() {
    float distance = length(g_FragPos.xyz - u_LightPos);
    distance = distance / u_FarPlane;
    gl_FragDepth = distance;
}
