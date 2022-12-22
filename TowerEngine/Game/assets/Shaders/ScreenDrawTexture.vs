attribute vec3 vertexPos;
attribute vec4 color;
attribute vec2 texCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

varying vec2 vTexCoords;

void main()
{
	vTexCoords = texCoords;
	gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}