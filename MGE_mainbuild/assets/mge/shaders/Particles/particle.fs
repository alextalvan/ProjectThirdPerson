#version 330
uniform sampler2D textureDiffuse;
in vec2 fTexCoord;

void main()
{
	gl_FragColor = texture(textureDiffuse,fTexCoord);
}
