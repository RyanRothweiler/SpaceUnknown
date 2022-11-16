precision mediump float;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 ID;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform sampler2D idTexture;

void main()
{
	float exposure = 1.0;

    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb * 0.15f;
  	hdrColor += bloomColor;
  
    // exposure tone mapping
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);


    FragColor = vec4(mapped, 1.0);
    //FragColor = vec4(hdrColor, 1.0);

    ID = vec4(1,0,0,1);
}