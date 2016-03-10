//DIFFUSE TEXTURE VERTEX SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

uniform	mat4 projectionMatrix;
uniform	mat4 viewMatrix;
uniform	mat4 modelMatrix;
uniform mat4 lightMatrixNear;
uniform mat4 lightMatrixFar;
uniform mat4 lightMatrixMid;
uniform bool hasNormalMap;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;

out vec3 FragPos;
out vec4 FragPosLightSpaceNear;
out vec4 FragPosLightSpaceFar;
out vec4 FragPosLightSpaceMid;
out vec2 TexCoord;
out vec3 Normal;
out mat3 TBN;

void main( void )
{
    //vertex position
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(vertex, 1.f);
	FragPos = vec3(modelMatrix * vec4(vertex, 1.f));
	//Normal = normalize(modelMatrix * vec4(normal,0.0f)).xyz;
	Normal = mat3(transpose(inverse(modelMatrix))) * normal;
	TexCoord = uv;

    if (hasNormalMap) {
        vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
        vec3 N = Normal; //normalize(vec3(modelMatrix * vec4(normal, 0.0))); //might be possible to replace with Normal
        // re-orthogonalize T with respect to N
        T = normalize(T - dot(T, N) * N);
        // then retrieve perpendicular vector B with the cross product of T and N
        vec3 B = cross(T, N);
        TBN = mat3(T, B, N);
    }

    FragPosLightSpaceNear = lightMatrixNear * vec4(FragPos, 1.0);
    FragPosLightSpaceFar = lightMatrixFar * vec4(FragPos, 1.0);
    FragPosLightSpaceMid = lightMatrixMid * vec4(FragPos, 1.0);
}




