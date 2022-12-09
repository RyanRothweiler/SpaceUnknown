attribute vec3 vertexPos;
attribute vec4 color;
attribute vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

varying vec4 vColor;
varying vec2 vTexCoords;

void main()
{
	vColor = color;
	vTexCoords = texCoords;

	gl_Position = projection * view * vec4(vertexPos, 1.0);
}