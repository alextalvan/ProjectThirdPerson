#version 330
uniform sampler2D frameTexture;
varying vec2 uv;

void main(void) {
  gl_FragColor = texture2D(frameTexture, uv) * vec4(1,0,0,1);
}
