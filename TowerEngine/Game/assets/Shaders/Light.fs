precision mediump float;

in vec4 vColor;
in vec2 vTexCoords;

out vec4 FragColor;

uniform sampler2D tex;

void main() 
{
	FragColor = vColor;
}