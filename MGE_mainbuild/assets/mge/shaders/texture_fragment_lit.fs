//DIFFUSE TEXTURE FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

uniform int lightCount;
uniform vec3 viewPos;
uniform bool hasNormalMap;
uniform bool hasSpecMap;

struct Material
{
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D specularMap;
	float smoothness;
    float shininess;
	float ambient;
};
uniform Material material;

struct Light
{
    int type;//0 = directional, 1= point
    vec3 position;
    vec3 direction;
    vec3 color;
    vec3 attenuation;
    float angle;
};
uniform Light LightArray[MGE_MAX_LIGHTS];

in vec2 texCoord;
in vec3 worldNormal;
in vec3 position;
in mat3 TBN;

out vec4 fragment_color;

vec3 DoDirectionalLight(int lightIndex, vec3 norm, vec3 viewDir);
vec3 DoPointLight(int lightIndex, vec3 norm, vec3 worldVertex, vec3 viewDir);
vec3 DoSpotlight(int lightIndex, vec3 norm, vec3 worldVertex, vec3 viewDir);

void main( void )
{
    vec3 normal = normalize(worldNormal);

    if (hasNormalMap) {
        normal = texture(material.normalMap, texCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    }

    vec3 viewDir = normalize(viewPos - position);

	vec3 outColor = vec3(0,0,0);

    //light handling
    for(int i=0;i<lightCount;++i)
    {
        if(LightArray[i].type==0)
            outColor += DoDirectionalLight(i, normal, viewDir);

        if(LightArray[i].type==1)
            outColor += DoPointLight(i, normal, position, viewDir);

        //if(LightArray[i].type==2)
        //    outColor += DoSpotlight(i, normal, position, viewDir);
    }

    vec3 ambient = vec3(texture(material.diffuseMap, texCoord)) * material.ambient;
    fragment_color = vec4(ambient + outColor,1.0f);
}

vec3 DoDirectionalLight(int lightIndex, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-LightArray[lightIndex].direction);

    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, texCoord)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, texCoord))) * spec * material.smoothness;
    }

    // Combine results
    return (diffuse + specular);
}

vec3 DoPointLight(int lightIndex, vec3 norm, vec3 worldVertex, vec3 viewDir)
{
    vec3 lightDir = normalize(LightArray[lightIndex].position - worldVertex);
    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, texCoord)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, texCoord))) * spec * material.smoothness;
    }

    // Attenuation
    float distance = length(LightArray[lightIndex].position - worldVertex);
    vec3 att = LightArray[lightIndex].attenuation;
    float attenuation = 1.0f / (att.x + att.y * distance + att.z * distance * distance);

	// Add attenuation
    diffuse *= attenuation;
    specular *= attenuation;

    // Combine results
    return (diffuse + specular);
}

vec3 DoSpotlight(int lightIndex, vec3 norm, vec3 worldVertex, vec3 viewDir)
{
    vec3 lightDir = normalize(LightArray[lightIndex].position - worldVertex);
    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, texCoord)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        vec3 specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, texCoord))) * spec * material.smoothness;
    }

    // Attenuation
    float distance = length(LightArray[lightIndex].position - worldVertex);
    vec3 att = LightArray[lightIndex].attenuation;
    float attenuation = 1.0f / (att.x + att.y * distance + att.z * distance * distance);

    // Spotlight intensity
    float cutOff = LightArray[lightIndex].angle - LightArray[lightIndex].angle / 5.0f;
    float outerCutOff = LightArray[lightIndex].angle + LightArray[lightIndex].angle / 5.0f;
    float theta = dot(lightDir, normalize(-LightArray[lightIndex].direction));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (diffuse + specular);
}
