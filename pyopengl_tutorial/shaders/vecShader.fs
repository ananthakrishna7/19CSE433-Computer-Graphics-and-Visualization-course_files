#version 330 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexColor;

out vec3 fragmentColor;

void main()
{
    gl_Position = vec4(vertexPos.x, vertexPos.y, vertexPos.z, 1.0);
    gl_PointSize = 10.0;
    fragmentColor = vertexColor;
}