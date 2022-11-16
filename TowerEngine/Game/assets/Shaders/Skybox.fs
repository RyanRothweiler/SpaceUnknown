precision highp float;

out vec4 FragColor;

in vec3 localPos;

uniform samplerCube skybox;

void main()
{    
    //FragColor = textureLod(skybox, localPos, 2);
    FragColor = texture(skybox, localPos);
}