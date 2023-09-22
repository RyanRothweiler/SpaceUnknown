precision mediump float;

varying vec4 vColor;
varying vec2 vTexCoords;

uniform sampler2D diffuseTex;

void main() 
{
	gl_FragColor = vColor * texture2D(diffuseTex, vTexCoords);
}
