precision highp float;

Expose uniform vec4 color; 
Expose uniform float ringSize; 
uniform vec3 PickingColor;

varying vec2 vTexCoords;

void main() 
{
	float distance = 1.0 - length(vTexCoords);
	float d2 = ringSize - length(vTexCoords);

	vec3 col = vec3(smoothstep(0.0, 0.001, distance));
	vec3 col2 = vec3(smoothstep(0.0, 0.001, d2));

	vec4 c = vec4(1, 1, 1, 1.0 - col2.x);
	gl_FragColor = vec4(col.x, col.y, col.z, 1.0 * c.w) * color;
}
