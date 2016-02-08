//DIFFUSE TEXTURE FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1

uniform sampler2D textureDiffuse;
in vec2 texCoord;
in vec3 v2f;
out vec4 fragment_color;

void main( void )
{
	//fragment_color = vec4(testColor,1);//
	fragment_color = texture(textureDiffuse,texCoord) * vec4(v2f,1);
}
