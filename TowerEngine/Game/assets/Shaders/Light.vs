precision mediump float;

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoords;
layout (location = 3) in vec3 normal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec3 lightPos;

out vec4 vColor;
out vec2 vTexCoords;

void main()
{
	vColor = color;
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}