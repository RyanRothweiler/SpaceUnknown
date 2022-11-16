layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 viewPosition;

uniform mat4 lightProjection;
uniform mat4 lightView;

out vec3 vNormal;
out vec3 vFragPos;
out vec3 vViewPos;
out mat4 vLightSpace;

void main()
{
	vFragPos = vec3(model * vec4(vertexPos, 1.0));
	vViewPos = viewPosition;
	vLightSpace = lightProjection * lightView;

	vNormal = normalize(mat3(transpose(inverse(model))) * normal);

	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}