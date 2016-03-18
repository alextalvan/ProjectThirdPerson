#version 330 compatibility
//layout(triangles) in;
//layout(triangle_strip, max_vertices=3) out;

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

uniform mat4 vpMatrix;
uniform	mat4 rotationMatrix;

in float gsLifetime[];
in float gsScale[];

out vec2 fTexCoord;

void main()
{
    if(gsLifetime[0]<0.0)
        return;


    mat4 rot = rotationMatrix;
    rot[0] *= gsScale[0];
    rot[1] *= gsScale[0];

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
