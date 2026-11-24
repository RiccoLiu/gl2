
#version 330 core

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_AOMap;

uniform vec3 u_LightsPos[4];
uniform vec3 u_LightColor[4];
uniform vec3 u_ViewPos;

const float PI = 3.14159265359;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

out vec4 o_FragColor;

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(u_NormalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.FragPos);
    vec3 Q2  = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N   = normalize(fs_in.Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main() {
    // o_FragColor = texture(u_AlbedoMap, fs_in.TexCoords);

    vec3 albedo = pow(texture(u_AlbedoMap, fs_in.TexCoords).rgb, vec3(2.2)); // gamma decoder
    vec3 normal = texture(u_NormalMap, fs_in.TexCoords).rgb;
    float metallic = texture(u_MetallicMap, fs_in.TexCoords).r;
    float roughness = texture(u_RoughnessMap, fs_in.TexCoords).r;
    float ao = texture(u_AOMap, fs_in.TexCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(u_ViewPos - fs_in.FragPos);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic); // 金属的基础反射率等于材质颜色

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 4; i++) {
        vec3 L = normalize(u_LightsPos[i] - fs_in.FragPos);
        vec3 H = normalize(V + L);

        float distance = length(u_LightsPos[i] - fs_in.FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = u_LightColor[i] * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF *G *F; 
        float denominator = 4.0 *max(dot(N, V), 0.0) *max(dot(N, L), 0.0) + 0.0001; //+ 0.0001 to prevent divide by zero
        vec3 specular = numerator /denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metallic; // 金属没有漫反射

        float NdotL = max(dot(N, L), 0.0); 
        Lo += (kD *albedo /PI + specular) *radiance *NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;    // 环境光遮蔽比率: AO
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));        // reinhard: 高光抑制色调映射
    color = pow(color, vec3(1.0/2.2));          // gamma encoder
    
    o_FragColor = vec4(color, 1.0);
}