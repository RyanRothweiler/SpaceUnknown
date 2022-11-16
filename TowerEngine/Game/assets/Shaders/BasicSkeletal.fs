precision highp float;

struct light {
    int Type;
    vec3 Direction;
};

in vec2 vTexCoords;
in vec3 vNormal;
in vec3 vNormalTan;
in vec3 vNormalBiTan;

uniform vec3 vLightPosition;

in vec3 vFragPos;
in vec3 vViewPos;
in mat4 vLightSpace;

Expose uniform vec3 ambientColor;
Expose uniform float ambientStrength;
uniform vec3 PickingColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 ID;

Expose uniform sampler2D diffuseTex;
Expose uniform sampler2D normalTex;
uniform sampler2D shadowMap;
uniform light Light;

float ShadowCalc(vec3 lightDir) {

	vec4 fragPosLightSpace = vLightSpace * vec4(vFragPos, 1.0);
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = (projCoords * 0.5) + 0.5;

	float depthFromShadowMap = texture(shadowMap, projCoords.xy).r;
	float currDepth = projCoords.z;

	float bias = max(0.1 * (1.0 - dot(vNormal, lightDir)), 0.05); 

	float shadow = currDepth - bias > depthFromShadowMap ? 0.8 : 0.0;

	return shadow;
}

void main() 
{

	mat3 tbn = mat3(vNormalTan, vNormalBiTan, vNormal);

	vec3 texNormal = texture(normalTex, vTexCoords).rgb;
	texNormal = (texNormal * 2.0) - 1.0;
	texNormal = normalize(tbn * texNormal);

	vec3 norm = normalize(texNormal);
	//vec3 lightDir = normalize(vLightPosition - vFragPos);
	vec3 lightDir = normalize(Light.Direction);
	float lightIntensity = 1.0;
	vec3 lightColor = vec3(1.0, 1.0, 1.0) * lightIntensity;

	// Specular
	float specularStrength = 0.2;
	vec3 viewDir = normalize(vViewPos - vFragPos);

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

	//vec3 halfwayDir = normalize(lightDir + viewDir);
	//float spec = pow(max(dot(norm, halfwayDir), 0.0), 256);

	vec3 specular = specularStrength * spec * lightColor;

	// Diffuse
	vec3 tex = vec3(texture(diffuseTex, vTexCoords));

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor * tex;

	// Ambient
	vec3 ambientColor = ambientColor * ambientStrength;

    // simple attenuation
	float distance = length(vLightPosition - vFragPos) / 100.0;
    float attenuation = 1.0 / distance;

    float shadow = ShadowCalc(lightDir);

    // Disable shadows
    shadow = 0.0;

	vec3 result = (diffuse + specular + ambientColor) * (1.0 - shadow);
	FragColor = vec4(result, 1.0);

	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);


	// Set brightness color
	float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0) {
        BrightColor = vec4(FragColor.rgb, 1.0);
	} else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    ID = vec4(PickingColor, 1.0);
}