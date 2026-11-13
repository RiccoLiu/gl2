
#version 330 core

uniform sampler2D u_Image;
uniform bool u_Horizontal;
uniform float u_Weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162); // 

in vec2 v_TexCoords;

out vec4 o_FragColor;

void main() {
    vec3 color = texture(u_Image, v_TexCoords).rgb;

    vec3 result = color * u_Weight[0];
    vec2 texSize = 1.0 / textureSize(u_Image, 0);

    if (u_Horizontal) {
        for (int i = 1; i < 5; i++) {
            result += texture(u_Image, v_TexCoords + vec2(i * texSize.x, 0.0)).rgb * u_Weight[i];
            result += texture(u_Image, v_TexCoords - vec2(i * texSize.x, 0.0)).rgb * u_Weight[i];
        }
    } else {
        for (int i = 1; i < 5; i++) {
            result += texture(u_Image, v_TexCoords + vec2(0.0, i * texSize.y)).rgb * u_Weight[i];
            result += texture(u_Image, v_TexCoords - vec2(0.0, i * texSize.y)).rgb * u_Weight[i];
        }
    }
    o_FragColor = vec4(result, 1.0);
}
