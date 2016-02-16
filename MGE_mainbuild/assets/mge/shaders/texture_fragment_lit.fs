//DIFFUSE TEXTURE FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

uniform sampler2D textureDiffuse;
uniform sampler2D normalMap;

in vec2 texCoord;
in vec3 worldNormal;
in vec3 position;
out vec4 fragment_color;

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
	//fragment_color = vec4(testColor,1);//
	//fragment_color = texture(textureDiffuse,texCoord) * vec4(v2f,1);

    vec3 normal = normalize(worldNormal);
	 // Obtain normal from normal map in range [0,1]
    normal = texture(normalMap, texCoord).rgb;
    // Transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);

	vec3 finalLight = vec3(0,0,0);

    //light handling
    for(int i=0;i<lightCount;++i)
    {
        if(LightArray[i].type==0)
            finalLight = DoDirectionalLight(i,finalLight,normal);

        if(LightArray[i].type==1)
            finalLight = DoPointLight(i,finalLight,normal,position);

        if(LightArray[i].type==2)
            finalLight = DoSpotlight(i,finalLight,normal,position);
    }

    fragment_color = texture(textureDiffuse,texCoord) * vec4(finalLight,1);
}
