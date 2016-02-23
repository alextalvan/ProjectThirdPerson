#version 330 core
layout (location = 0) in vec2 vertex;
varying out vec2 uv;

void main()
{
    gl_Position = vec4(vertex, 0.0, 1.0);
    uv = (vertex + 1.0) / 2.0;
}
