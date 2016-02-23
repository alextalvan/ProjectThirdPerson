//DIFFUSE TEXTURE VERTEX SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1

uniform mat4 vpMatrix;
uniform	mat4 rotationMatrix;
uniform vec3 position;

in vec3 vertex;
in vec3 normal;
in vec2 uv;

out vec2 texCoord; //make sure the texture coord is interpolated

void main( void )
{
    vec4 pos = rotationMatrix * vec4(vertex,1.0f) + vec4(position,0.0f);
	gl_Position = vp * pos;
	texCoord = uv;
}
