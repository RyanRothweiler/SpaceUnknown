precision highp float;

Expose uniform vec4 color; 
uniform vec3 PickingColor;

varying vec2 vTexCoords;

void main() 
{
	float distance = 1.0 - length(vTexCoords);

	vec3 col = vec3(smoothstep(0.0, 0.001, distance));
	gl_FragColor = vec4(col.x, col.y, col.z, 1.0) * color;
}