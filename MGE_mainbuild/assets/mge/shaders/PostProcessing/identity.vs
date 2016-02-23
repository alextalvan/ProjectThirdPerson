#version 330
attribute vec2 vertex;
uniform sampler2D frameTexture;
varying vec2 uv;

void main(void)
{
  gl_Position = vec4(vertex, 0.0, 1.0);
  uv = (vertex + 1.0) / 2.0;
}
