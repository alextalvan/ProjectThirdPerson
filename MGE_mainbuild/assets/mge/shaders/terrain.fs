//DIFFUSE TEXTURE FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1

uniform sampler2D splatMap;
uniform sampler2D diffuse0;
uniform sampler2D diffuse1;
uniform sampler2D diffuse2;
uniform sampler2D diffuse3;

in vec2 texCoord;
out vec4 fragment_color;

void main( void )
{
    vec4 splat = texture(splatMap,texCoord);

    vec4 color = texture(diffuse0,texCoord*10) * splat.x + texture(diffuse1,texCoord*20) * splat.y + texture(diffuse2,texCoord*20) * splat.z + texture(diffuse3,texCoord*100) * splat.w;
	fragment_color = color;
}
