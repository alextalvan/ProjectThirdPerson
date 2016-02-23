#version 330 core
out vec4 color;
in vec2 uv;

uniform sampler2D frameTexture;

void main()
{
    const float gamma = 1.0;
    vec3 hdrColor = texture(frameTexture, uv).rgb;

    // reinhard
    vec3 result = hdrColor / (hdrColor + vec3(1.0));
    // exposure
    //vec3 result = vec3(1.0) - exp(-hdrColor * 1.0f);
    // also gamma correct while we're at it
    result = pow(result, vec3(1.0 / gamma));
    color = vec4(result, 1.0f);
}
