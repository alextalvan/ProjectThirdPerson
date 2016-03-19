#version 330
uniform mat4 vpMatrix;//view and projection combined
uniform	mat4 rotationMatrix;//camera orientation


layout (location = 0) in vec3 position;
layout (location = 1) in vec2 particleInfo;//x stores lifetime, y stores scale


out vec2 gsParticleInfo;

void main()
{
    gl_Position = vec4(position,0.0f);
    gsParticleInfo = particleInfo;
}
