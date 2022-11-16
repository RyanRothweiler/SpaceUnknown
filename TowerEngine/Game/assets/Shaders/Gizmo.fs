precision highp float;

Expose uniform vec4 color; 
uniform vec3 PickingColor;

layout (location = 0) out vec4 FragColor;
//layout (location = 1) out float gl_FragDepth;
layout (location = 2) out vec4 ID;

void main() 
{
	FragColor = color;
    ID = vec4(PickingColor,1.0);
    gl_FragDepth = 0.0;
}