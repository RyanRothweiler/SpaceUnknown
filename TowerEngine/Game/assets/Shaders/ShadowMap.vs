precision highp float;

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 normalTan;
layout (location = 5) in vec3 normalBiTan;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec4 ambientColor;
uniform vec3 lightPosition;
uniform vec3 viewPosition;

out vec4 vColor;
out vec2 vTexCoords;
out vec4 vAmbientColor;
out vec3 vLightPosition;
out vec3 vNormal;
out vec3 vFragPos;
out vec3 vViewPos;
out vec3 vNormalTan;
out vec3 vNormalBiTan;

void main()
{
	vAmbientColor = ambientColor;
	vColor = color;
	vTexCoords = texCoords;

	vLightPosition = lightPosition;

	vFragPos = vec3(model * vec4(vertexPos, 1.0));

	//vNormal = mat3(model) * normal;
	//vNormalTan = mat3(model) * normalTan;
	//vNormalBiTan = mat3(model) * normalBiTan;

	vNormal = normalize(vec3(model * vec4(normal, 0.0)));
	vNormalTan = normalize(vec3(model * vec4(normalTan, 0.0)));
	vNormalBiTan = normalize(vec3(model * vec4(normalBiTan, 0.0)));

	vViewPos = viewPosition;

	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}