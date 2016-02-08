//DIFFUSE TEXTURE VERTEX SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1

uniform	mat4 	projectionMatrix;
uniform	mat4 	viewMatrix;
uniform	mat4 	modelMatrix;

uniform sampler2D heightMap;

in vec3 vertex;
in vec3 normal;
in vec2 uv;

out vec2 texCoord; //make sure the texture coord is interpolated

void main( void )
{
    //vertex.y -= texture(heightMap,uv).r * 0.001f;
    vec4 pos = projectionMatrix * viewMatrix * modelMatrix * vec4(vertex, 1.f);
    //pos.y += texture(heightMap,uv).r * 5;
	gl_Position = pos;
	texCoord = uv;
}
