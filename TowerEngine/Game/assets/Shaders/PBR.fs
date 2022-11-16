precision highp float;

uniform vec3 vLightPosition;
in vec3 vFragPos;
in vec3 vViewPos;
in mat4 vLightSpace;
in vec2 vTexCoords;
in vec3 WorldPos;
in vec3 vNormal;
in vec3 vNormalTan;
in vec3 vNormalBiTan;

Expose uniform sampler2D albedoMap;
Expose uniform sampler2D metallicMap;
Expose uniform sampler2D roughnessMap;
Expose uniform sampler2D normalMap;

Expose uniform sampler2D bakedAOMap;

Expose uniform float ao;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT; 
uniform vec3 PickingColor;
uniform vec3 flatColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 ID;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform int lightType[4];
uniform vec3 lightDirections[4];
uniform int lightsCount;

uniform sampler2D shadowMap;

float PI = 3.14159265359;

float ShadowCalc(vec3 lightDir) {

	vec4 fragPosLightSpace = vLightSpace * vec4(vFragPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = (projCoords * 0.5) + 0.5;

	float depthFromShadowMap = texture(shadowMap, projCoords.xy).r;
	float currDepth = projCoords.z;

	float bias = max(0.1 * (1.0 - dot(vNormal, lightDir)), 0.01); 

	float shadow = currDepth - bias > depthFromShadowMap ? 0.8 : 0.0;

	return shadow;
}

/*
float CalculateAttenuation() {
    float distance    = length(vLightPosition - vFragPos);
    return attenuation = 1.0 / (light.constant + light.linear * distance +  light.quadratic * (distance * distance));
}
*/

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

void main() 
{
    mat3 tbn = mat3(vNormalTan, vNormalBiTan, vNormal);

    vec3 N = normalize(vNormal);
    N = texture(normalMap, vTexCoords).rgb;
    N = (N * 2.0) - 1.0;
    N = normalize(tbn * N);

    vec3 V = normalize(vViewPos - WorldPos);
    vec3 F0 = vec3(0.04); 

    vec3 R = reflect(-V, N); 

    // Convert from srgb to linear
    vec3 albedo = texture(albedoMap, vTexCoords).rgb;
    vec3 bakedAO = texture(bakedAOMap, vTexCoords).rgb;
    float metallic = texture(metallicMap, vTexCoords).r;
    float roughness = texture(roughnessMap, vTexCoords).r;

    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < lightsCount; i++) {

        vec3 L = vec3(0.0);
        if (lightType[i] == LightType_Point) {
            L = normalize(lightPositions[i] - WorldPos);
        } else if (lightType[i] == LightType_Directional) { 
            L = normalize(lightDirections[i]);
        }

        vec3 H = normalize(V + L);

        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        if (lightType[i] == LightType_Directional) {
            attenuation = 1.0;
        }
        vec3 radiance = lightColors[i] * attenuation;

        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

        float NDF = DistributionGGX(N, H, roughness);       
        float G = GeometrySmith(N, V, L, roughness); 

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
  
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);        
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 kS = fresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y); 

    vec3 ambient = (kD * diffuse + specular) * ao;
    //ambient = vec3(0.03) * albedo * ao;

    vec3 color = ((ambient) + Lo) * bakedAO;

    color = color / (color + vec3(1.0)); 
    color = color + flatColor;


    // gamma correction
    float gamma = 2.2;
    color = pow(color.rgb, vec3(1.0/gamma));


    FragColor = vec4(color, 1.0);


    // Set brightness color
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        BrightColor = vec4(FragColor.rgb, 1.0);
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    // Set the picker color
    ID = vec4(PickingColor,1.0);
}