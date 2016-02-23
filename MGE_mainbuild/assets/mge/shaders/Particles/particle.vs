#version 330

uniform mat4 vpMatrix;//view and projection combined
uniform	mat4 rotationMatrix;//camera orientation
uniform vec3 position;//this is passed separately for each particle
//uniform mat4 debug;

in vec3 vertex;
in vec3 normal;
in vec2 uv;

out vec2 texCoord; //interpolated uv

void main( void )
{
    vec4 pos = rotationMatrix * vec4(vertex,1.0f) + vec4(position,0.0f);
	gl_Position = vpMatrix * pos;
	texCoord = uv;
}
