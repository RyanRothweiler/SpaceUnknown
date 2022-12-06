precision mediump float;

varying vec2 vTexCoords;

uniform sampler2D diffuseTex;
uniform vec4 color;

void main() 
{
	gl_FragColor = texture2D(diffuseTex, vTexCoords) * color;
}