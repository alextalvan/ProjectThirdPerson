#version 330 core

layout (location = 0) in vec3 vertex;

uniform mat4 lightMatrix;
uniform mat4 modelMatrix;

//out vec3 pos;

void main()
{
    gl_Position = lightMatrix * modelMatrix * vec4(vertex, 1.0f);

    //test
    //pos = lightMatrix * modelMatrix * vec4(vertex, 1.0f);
    //gl_Position = pos;
}
