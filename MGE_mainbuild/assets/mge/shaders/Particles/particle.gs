#version 330
//layout(triangles) in;
//layout(triangle_strip, max_vertices=3) out;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

uniform mat4 vpMatrix;
uniform	mat4 rotationMatrix;

in vec2 gsParticleInfo[];//x is lifetime, y is scale

out vec2 fTexCoord;
//out float debug;

void main()
{
    if(gsParticleInfo[0].x<0.0)
        return;

    mat4 rot = rotationMatrix;
    rot[0] *= gsParticleInfo[0].y;
    rot[1] *= gsParticleInfo[0].y;

    //first quad vert
    gl_Position = vpMatrix * (gl_in[0].gl_Position + rot * vec4(0.5,0.5,0,1));
    fTexCoord = vec2(0,1);
    EmitVertex();

    //second quad vert
    gl_Position = vpMatrix * (gl_in[0].gl_Position + rot * vec4(0.5,-0.5,0,1));
    fTexCoord = vec2(0,0);
    EmitVertex();

    //third quad vert
    gl_Position = vpMatrix * (gl_in[0].gl_Position + rot * vec4(-0.5,0.5,0,1));
    fTexCoord = vec2(1,1);
    EmitVertex();

    //fourth quad vert
    gl_Position = vpMatrix * (gl_in[0].gl_Position + rot * vec4(-0.5,-0.5,0,1));
    fTexCoord = vec2(1,0);
    EmitVertex();

    EndPrimitive();

}
