uniform sampler2D frameTexture;
varying vec2 uv;

void main(void)
{
    if(uv.x <=0.5)
        gl_FragColor = vec4(0,0,0,1);
    else
        gl_FragColor = texture2D(frameTexture, uv);
}
