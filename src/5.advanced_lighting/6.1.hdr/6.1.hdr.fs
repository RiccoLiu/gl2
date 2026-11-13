#version 330 core

uniform sampler2D u_HDRBuffer;
uniform bool u_HDR;
uniform float u_Exposure;

in vec2 v_TexCoords;

out vec4 o_FragColor;

void main() {         
    const float gamma = 2.2;
    vec3 hdrColor = texture(u_HDRBuffer, v_TexCoords).rgb;
    if(u_HDR) {
        // reinhard: 过曝抑制色调映射
        // vec3 result = hdrColor / (hdrColor + vec3(1.0));
        // exposure: 曝光控制
        vec3 result = vec3(1.0) - exp(-hdrColor * u_Exposure);
        // 最后调整 GAMMA       
        result = pow(result, vec3(1.0 / gamma));
        o_FragColor = vec4(result, 1.0);
    }
    else
    {
        vec3 result = pow(hdrColor, vec3(1.0 / gamma));
        o_FragColor = vec4(result, 1.0);
    }
}