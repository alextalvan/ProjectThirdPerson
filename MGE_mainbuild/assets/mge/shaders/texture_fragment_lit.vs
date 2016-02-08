//DIFFUSE TEXTURE VERTEX SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

uniform	mat4 	projectionMatrix;
uniform	mat4 	viewMatrix;
uniform	mat4 	modelMatrix;

in vec3 vertex;
in vec3 normal;
in vec2 uv;

out vec2 texCoord; //make sure the texture coord is interpolated
out vec3 worldNormal;
out vec3 position;


void main( void )
{
    //vertex position
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(vertex, 1.f);
	position = vec3(modelMatrix * vec4(vertex, 1.f));
	worldNormal = normalize((modelMatrix * vec4(normal,0)).xyz);
	texCoord = uv;
}




