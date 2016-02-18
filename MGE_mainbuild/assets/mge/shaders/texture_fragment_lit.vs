//DIFFUSE TEXTURE VERTEX SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

uniform	mat4 projectionMatrix;
uniform	mat4 viewMatrix;
uniform	mat4 modelMatrix;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

out vec2 texCoord;
out vec3 worldNormal;
out vec3 position;
out mat3 TBN;

void main( void )
{
    //vertex position
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(vertex, 1.f);
	position = vec3(modelMatrix * vec4(vertex, 1.f));
	worldNormal = normalize((modelMatrix * vec4(normal,0)).xyz);
	texCoord = uv;
    vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(modelMatrix * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(modelMatrix * vec4(normal, 0.0)));
    mat3 TBN = mat3(T, B, N);
}




