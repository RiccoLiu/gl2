- [1. PBR - Physically Based Rendering](#1-pbr---physically-based-rendering)
- [2. PBR渲染方程](#2-pbr渲染方程)
- [3. BRDF模型 Cook-Torrance](#3-brdf模型-cook-torrance)
- [4. PBR 材质](#4-pbr-材质)
- [5. PBR 着色器](#5-pbr-着色器)
- [IBL 贴图](#ibl-贴图)

## 1. PBR - Physically Based Rendering
 
基于物理的渲染，需要满足下面三个条件:

- 基于微平面的表面模型
- 能量守恒，反射光照强度不能超过入射强度
- 应用基于物理的BRDF

Metallic-Roughness 模型：

## 2. PBR渲染方程

$$
    L_o(p, \omega_o) = \int_{\Omega} f_r(p, \omega_i, \omega_o)\, L_i(p, \omega_i)\, n \cdot \omega_i \, d\omega_i
$$

- $\omega_o$: 观测方向
- $\omega_i$: 入射光方向
- $L_o(p, \omega_o) $: 点P出射辐照亮度 点
- $f_r(p, \omega_i, \omega_o)$: BRDF(双向反射分布函数) 用来描述不同材质将入射光反射到观测方向有多少
- $L_i(p, \omega_i)$: 入射光的辐照度
- $n \cdot \omega_i$: 入射光与发现的夹角
- $\int_{\Omega} ... d\omega_i$: 对点p半球$\Omega$ 内所欲的入射光$\omega_i$方向进行积分求和

## 3. BRDF模型 Cook-Torrance

$$
    f_r = k_d f_{lambert} + k_s f_{cook-torrance}
$$

- $k_d$: 被折射的能量占比
- $f_{lambert}$: lambert 漫反射
- $k_s$: 被反射的能量占比
- $f_{cook-torrance}$: cook-torrance 镜面反射

$$
    f_{lambert} = \frac{c}{\pi}
$$
- $c$:表面颜色
- $\pi$: 对颜色进行归一化
$$
    f_{cook-torrance} = \frac{DFG}{4(\omega_o \cdot n)(\omega_i \cdot n)}
$$

- D: 法线分布函数-根据微平面理论下的法线分布，估算有多少光反射到观察方向。
- F: 菲涅尔方程-描述不同材料下，反射光所占的比率。
- G: 几何遮挡函数-根据微平面理论，粗糙的平面会对其他平面产生遮挡。

D 法线分布函数：
$$
    NDF_{GGXTR}(n, h, \alpha) = \frac{\alpha^2}{\pi((n \cdot h)^2(\alpha^2 - 1) + 1)^2}
$$

- $n$: 微平面法向量
- $h$: 入射光线与观测方向的半程向量
- $\alpha$: 粗糙度-粗糙度低时，与半程向量一致的平面会集中在小块区域，导致小块高亮；

F 菲尼尔方程：

$$
    F_{Schlick}(h, v, F_0) = F_0 + (1 - F_0)(1 - (h \cdot v))^5
$$

- $h$: 入射光线与观测方向的半程向量
- $v$: 观察方向
- $F_0$: 基础反射率
- $F_{Schlick}(h, v, F_0)$: 镜面反射所占的比率(0, 1), 

G 几何遮挡函数：

$$
    G_{SchlickGGX}(n, v, k) = \frac{n \cdot v}{(n \cdot v)(1 - k) + k} 
$$

- $ n $: 微平面法向量
- $ v $: 观测方向
- $ k $: k是对 粗糙度 $ \alpha $ 的重映射，映射方式取决于是直接光照还是针对IBL光照。

$$
    k_{direct} = \frac{(\alpha + 1)^2}{8}
$$

$$
    k_{IBL} = \frac{\alpha^2}{2}
$$

几何遮挡部分可以分为，入射光线遮挡 $G_{sub}(n, l, k)$ 和 反射光线遮挡 $ G_{sub}(n, v, k) $, 这两种情况合并后即为G 几何遮挡函数：

$$   
    G(n, v, l, k) = G_{sub}(n, v, k)\, G_{sub}(n, l, k) 
$$

BRDF cook-torrance 模型:
$$
    L_o(p, \omega_o) = \int_\Omega \left( k_d \frac{c}{\pi} + k_s \frac{DFG}{4(\omega_o \cdot n)(\omega_i \cdot n)} \right) L_i(p, \omega_i)\, n \cdot \omega_i \, d\omega_i
$$

## 4. PBR 材质

- 反照率(Albedo)
- 法线(Normal)
- 金属(Metallic)
- 粗糙度(Roughness)
- AO 环境光遮蔽(Ambient Occlusion) 

## 5. PBR 着色器

```
void main() {		
    vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, TexCoords).r;
    float roughness = texture(roughnessMap, TexCoords).r;
    float ao        = texture(aoMap, TexCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);

    vec3 F0 = vec3(0.04);               // 非金属基础反射率: 0.04
    F0 = mix(F0, albedo, metallic);     // 根据金属性插值出基础反射率

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 4; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;       // 衰减

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        vec3 kS = F;                                        // 根据菲涅尔方程近似计算镜面反射的比率

        vec3 kD = vec3(1.0) - kS;                           // 能量守恒计算漫反射的比率
        kD *= 1.0 - metallic;	                            // 金属材质的漫反射比率为0， 根据金属性插值出漫反射比率

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    vec3 ambient = vec3(0.03) * albedo * ao;                // 环境光比率默认:0.03; 环境光遮蔽比率: AO    
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));                    // 色调映射
    color = pow(color, vec3(1.0/2.2));                      // gamma校正 

    FragColor = vec4(color, 1.0);
}
```

## IBL 贴图

