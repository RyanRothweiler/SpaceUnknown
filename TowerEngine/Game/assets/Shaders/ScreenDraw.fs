precision highp float;

Expose uniform vec4 color; 
uniform vec3 PickingColor;

//varying vec4 FragColor;
//varying vec4 ID;

void main() 
{
	gl_FragColor = color;
    //ID = vec4(PickingColor,1.0);
}