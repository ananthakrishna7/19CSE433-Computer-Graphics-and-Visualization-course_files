#version 330 core
layout (location = 0) in vec3 vertexPos;

void main()
{
    gl_Position = vec4(vertexPos.x, vertexPos.y, 0.0, 1.0);
    gl_PointSize = 1.0;
}