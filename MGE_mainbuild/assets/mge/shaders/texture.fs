//DIFFUSE TEXTURE FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1

uniform sampler2D textureDiffuse;
uniform vec3 color;
in vec2 texCoord;
out vec4 fragment_color;

void main( void ) {
	fragment_color = texture(textureDiffuse,texCoord) * vec4(color,1);
}
