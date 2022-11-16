layout (location = 0) in vec3 vertexPos;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    localPos = vertexPos;
    gl_Position =  projection * view * vec4(vertexPos, 1.0);
}