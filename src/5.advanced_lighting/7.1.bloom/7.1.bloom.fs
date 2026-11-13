
#version 330 core

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_BloomMap;
uniform bool u_Bloom;
uniform float u_Exposure;

in vec2 v_TexCoords;

out vec4 o_FragColor;

void main() {
    // o_FragColor = texture(u_DiffuseMap, v_TexCoords);

    vec3 diffuseColor = texture(u_DiffuseMap, v_TexCoords).rgb;
    vec3 bloomColor = texture(u_BloomMap, v_TexCoords).rgb;

    // 1. combine
    if (u_Bloom) {
        diffuseColor += bloomColor;
    }

    // 2. tone mapping
    diffuseColor = 1 - exp(-diffuseColor * u_Exposure);

    // 3. gamma correction
    float gamma = 2.2;
    diffuseColor = pow(diffuseColor, vec3(1.0 / gamma));

    o_FragColor = vec4(diffuseColor, 1.0);
}
