precision highp float;

Expose uniform vec3 radiusCenter[256];
Expose uniform float radius[256];
Expose uniform int radiusCenterCount;

uniform vec3 PickingColor;

const float grey = 0.05;
const int maxRadiusCount = 100;

void main() 
{
	float fade = 1.0;

	for (int i = 0; i < maxRadiusCount; i++) {
		if (i < radiusCenterCount) {
			float dist = distance(vec4(radiusCenter[i], 0), gl_FragCoord);
			float f = 1.0 - smoothstep(radius[i], radius[i] - 0.1, dist);
			fade = min(f, fade);
		}
	}

	gl_FragColor = vec4(grey, grey, grey, fade);
}
