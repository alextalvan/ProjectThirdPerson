#version 330 compatibility
uniform mat4 vpMatrix;//view and projection combined
uniform	mat4 rotationMatrix;//camera orientation


in vec3 position;
in float lifetime;
in float scale;

out float gsLifetime;
out float gsScale;

void main( void )
{
    gl_Position = vec4(position,0.0f);
    gsLifetime = lifetime;
    gsScale = scale;
}
