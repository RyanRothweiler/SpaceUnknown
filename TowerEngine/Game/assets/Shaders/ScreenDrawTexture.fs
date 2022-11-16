precision mediump float;

in vec2 vTexCoords;

out vec4 FragColor;

uniform sampler2D diffuseTex;
uniform vec4 color;

void main() 
{
	FragColor = texture(diffuseTex, vTexCoords) * color;
}