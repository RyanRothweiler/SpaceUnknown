layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 normalTan;
layout (location = 4) in vec3 normalBiTan;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 viewPosition;

uniform mat4 lightProjection;
uniform mat4 lightView;

out vec4 vColor;
out vec2 vTexCoords;
out vec3 vNormal;
out vec3 vFragPos;
out vec3 vViewPos;
out vec3 vNormalTan;
out vec3 vNormalBiTan;
out mat4 vLightSpace;

void main()
{
	vTexCoords = texCoords;

	vFragPos = vec3(model * vec4(vertexPos, 1.0));

	//vNormal = mat3(model) * normal;
	//vNormalTan = mat3(model) * normalTan;
	//vNormalBiTan = mat3(model) * normalBiTan;

	vNormal = normalize(mat3(transpose(inverse(model))) * normal);
	vNormalTan = normalize(vec3(model * vec4(normalTan, 0.0)));
	vNormalBiTan = normalize(vec3(model * vec4(normalBiTan, 0.0)));

	vViewPos = viewPosition;

	vLightSpace = lightProjection * lightView;

	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}