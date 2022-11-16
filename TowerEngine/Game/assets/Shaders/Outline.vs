const int MAX_JOINTS = 50;
const int MAX_WEIGHTS = 3;

layout (location = 0) in vec3 vertexPos;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 normalTan;
layout (location = 5) in vec3 normalBiTan;

layout (location = 6) in vec3 boneWeights;
layout (location = 7) in vec3 boneIndecies;


uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 viewPosition;

uniform mat4 lightProjection;
uniform mat4 lightView;
uniform mat4 boneTransforms[MAX_JOINTS];

Expose uniform float thickness;

out vec2 vTexCoords;
//out vec3 vLightPosition;
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

	//vNormalTan = normalize(vec3(model * vec4(normalTan, 0.0)));
	//vNormalBiTan = normalize(vec3(model * vec4(normalBiTan, 0.0)));

	vViewPos = viewPosition;

	vLightSpace = lightProjection * lightView;


	vec4 totalLocalPos = vec4(0.0);
	vec4 totalNormal = vec4(0.0);
	vec4 totalTan = vec4(0.0);
	vec4 totalBi = vec4(0.0);
	for (int i = 0; i < MAX_WEIGHTS; i++) {
		int boneIndex = int(boneIndecies[i]);
		if (boneIndex >= 0) {
			vec4 p = vec4(vertexPos, 1.0) * (boneWeights[i] * boneTransforms[boneIndex]);
			totalLocalPos += p;
			
			vec4 n = vec4(normal, 0.0) * (boneWeights[i] * boneTransforms[boneIndex]);
			totalNormal += n;

			vec4 t = vec4(normalTan, 0.0) * (boneWeights[i] * boneTransforms[boneIndex]);
			totalTan += t;

			vec4 b = vec4(normalBiTan, 0.0) * (boneWeights[i] * boneTransforms[boneIndex]);
			totalBi += b;
		}
	}


	vNormal = 		normalize(mat3(transpose(inverse(model))) * vec3(totalNormal.xyz));
	vNormalTan = 	normalize(mat3(transpose(inverse(model))) * vec3(totalTan.xyz));
	vNormalBiTan = 	normalize(mat3(transpose(inverse(model))) * vec3(totalBi.xyz));

	//totalLocalPos = totalLocalPos + (totalNormal * thickness);
	gl_Position = projection * view * model * vec4(totalLocalPos.xyz, 1.0);
}