attribute vec3 vertexPos;
attribute vec2 texCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

varying vec2 vTexCoords;

void main()
{
	vTexCoords = texCoords;
    gl_Position = projection * view * vec4(vertexPos, 1.0);
    gl_Position.z *= 0.5;
}