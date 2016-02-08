//DIFFUSE TEXTURE VERTEX SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

uniform	mat4 	projectionMatrix;
uniform	mat4 	viewMatrix;
uniform	mat4 	modelMatrix;

in vec3 vertex;
in vec3 normal;
in vec2 uv;

out vec2 texCoord; //make sure the texture coord is interpolated

out vec3 v2f;

uniform int lightCount;

struct light
{
    int type;//0 = directional, 1= point
    vec3 position;
    vec3 direction;
    vec3 color;
    vec3 attenuation;
    float angle;
};

uniform light LightArray[MGE_MAX_LIGHTS];

vec3 DoDirectionalLight(int lightIndex, vec3 currentLight, vec3 worldNormal)
{
    vec3 diffuseTerm = ((dot(worldNormal,LightArray[lightIndex].direction)+1.0f) * 0.5f) * LightArray[lightIndex].color;
    return currentLight + diffuseTerm;
}

vec3 DoPointLight(int lightIndex, vec3 currentLight, vec3 worldNormal,vec3 worldVertex)
{
    vec3 diff = worldVertex - LightArray[lightIndex].position;
    float dist = length(diff);
    vec3 direction = normalize(diff);
    vec3 diffuseTerm = ((dot(worldNormal,direction)+1.0f) * 0.5f) * LightArray[lightIndex].color;
    vec3 att = LightArray[lightIndex].attenuation;
    diffuseTerm /= att.x + att.y * dist + att.z * dist * dist;
    return currentLight + diffuseTerm;
}

vec3 DoSpotlight(int lightIndex, vec3 currentLight, vec3 worldNormal,vec3 worldVertex)
{
    vec3 diff = worldVertex - LightArray[lightIndex].position;
    float dist = length(diff);
    vec3 direction = normalize(diff);
    //check if in cone
    float angle = acos(dot(direction,normalize(LightArray[lightIndex].direction)));
    if(angle > LightArray[lightIndex].angle)
        return currentLight;

    vec3 diffuseTerm = ((dot(worldNormal,direction)+1.0f) * 0.5f) * LightArray[lightIndex].color;
    vec3 att = LightArray[lightIndex].attenuation;
    diffuseTerm /= att.x + att.y * dist + att.z * dist * dist;
    return currentLight + diffuseTerm;
}

void main( void )
{
    //vertex position
    mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(vertex, 1.f);

    vec3 finalLight = vec3(0,0,0);
    vec3 worldNormal = normalize((modelMatrix * vec4(normal,0)).xyz);
    vec3 worldVertex = (modelMatrix * vec4(vertex, 1.f)).xyz;

    //light handling
    for(int i=0;i<lightCount;++i)
    {
        if(LightArray[i].type==0)
            finalLight = DoDirectionalLight(i,finalLight,worldNormal);

        if(LightArray[i].type==1)
            finalLight = DoPointLight(i,finalLight,worldNormal,worldVertex);

        if(LightArray[i].type==2)
            finalLight = DoSpotlight(i,finalLight,worldNormal,worldVertex);
    }


	texCoord = uv;
	v2f = finalLight;
}




