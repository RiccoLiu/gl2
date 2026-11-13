#version 330 core

struct Light {
    vec3 Position;
    vec3 Color;
};

uniform Light u_Lights[16];
uniform sampler2D u_DiffuseTexture;
uniform vec3 u_ViewPos;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

out vec4 o_FragColor;

void main() {           
    vec3 color = texture(u_DiffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    // ambient
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 4; i++)
    {
        // diffuse
        vec3 lightDir = normalize(u_Lights[i].Position - fs_in.FragPos);
        float diff = max(dot(lightDir, normal), 0.0);
        vec3 diffuse = u_Lights[i].Color * diff * color;      
        vec3 result = diffuse;        
        // attenuation (use quadratic as we have gamma correction)
        float distance = length(fs_in.FragPos - u_Lights[i].Position);
        result *= 1.0 / (distance * distance);
        lighting += result;
                
    }
    o_FragColor = vec4(ambient + lighting, 1.0);
}