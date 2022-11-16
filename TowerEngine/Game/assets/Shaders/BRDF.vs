layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 texCoords;

out vec3 localPos;

void main()
{
    localPos = texCoords;
    gl_Position = vec4(vertexPos, 1.0);
} 