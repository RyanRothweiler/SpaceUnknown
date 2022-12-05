precision mediump float;

varying vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D image;
  
uniform bool horizontal;
//float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
float wight = 0.227027;

void main()
{             
    /*
    float tox = 1.0 / float(textureSize(image, 0).x);
    float toy = 1.0 / float(textureSize(image, 0).y);

    vec2 tex_offset = vec2(tox, toy);

    vec3 result = texture(image, TexCoords).rgb * weight; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(tex_offset.x * float(i), 0.0)).rgb * weight;
            result += texture(image, TexCoords - vec2(tex_offset.x * float(i), 0.0)).rgb * weight;
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * float(i))).rgb * weight;
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * float(i))).rgb * weight;
        }
    }
    FragColor = vec4(result, 1.0);
    */
}