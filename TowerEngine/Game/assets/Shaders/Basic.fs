precision highp float;

struct light {
    int Type;
    vec3 Direction;
};

uniform vec3 vLightPosition;
in vec3 vNormal;
in vec3 vFragPos;
in vec3 vViewPos;
in mat4 vLightSpace;

Expose uniform vec3 ambientColor;
Expose uniform float ambientStrength;
uniform vec3 PickingColor;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
layout (location = 2) out vec4 ID;

uniform sampler2D shadowMap;
uniform light Light;

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

void main() 
{
	vec3 lightDir = normalize(Light.Direction);

	float lightIntensity = 2.0f;

	vec3 lightColor = vec3(1.0, 1.0, 1.0) * lightIntensity;

	// Specular
	float specularStrength = 0.2;
	vec3 viewDir = normalize(vViewPos - vFragPos);

	vec3 norm = normalize(vNormal);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

	vec3 specular = specularStrength * spec * lightColor;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	// Ambient
	vec3 ambientColor = ambientColor * ambientStrength;

    // simple attenuation
	//float distance = length(vLightPosition - vFragPos) / 100.0;
    //float attenuation = 1.0 / distance;

    float shadow = ShadowCalc(lightDir);

    // Disable shadows!!
    shadow = 0.0;

	vec3 result = (diffuse + specular + ambientColor) * (1.0 - shadow);
	FragColor = vec4(result, 1.0);

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