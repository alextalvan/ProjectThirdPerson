//DIFFUSE TEXTURE FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

uniform int lightCount;
uniform vec3 viewPos;

struct Material
{
    sampler2D diffuseMap;
    sampler2D normalMap;
    sampler2D specularMap;
    float smoothness;
    float shininess;
    float ambient;
    bool hasNormalMap;
    bool hasSpecMap;
};
uniform Material material;

struct Light
{
    int type;
    vec3 position;
    vec3 direction;
    vec3 color;
    vec3 attenuation;
    float angle;
};
uniform Light LightArray[MGE_MAX_LIGHTS];

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;
in mat3 TBN;

out vec4 fragment_color;

vec3 DoDirectionalLight(int lightIndex, vec3 norm, vec3 viewDir);
vec3 DoPointLight(int lightIndex, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 DoSpotlight(int lightIndex, vec3 norm, vec3 fragPos, vec3 viewDir);

void main( void )
{
    vec3 normal = normalize(Normal);

    if (material.hasNormalMap) {
        normal = texture(material.normalMap, TexCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    }

    vec3 viewDir = vec3(0,0,0);
    if (material.hasSpecMap)
        viewDir = normalize(viewPos - FragPos);

	vec3 outColor = vec3(0,0,0);

    //light handling
    for(int i=0;i<lightCount;++i)
    {
        if(LightArray[i].type==0)
            outColor += DoDirectionalLight(i, normal, viewDir);

        if(LightArray[i].type==1)
            outColor += DoPointLight(i, normal, FragPos, viewDir);

        //if(LightArray[i].type==2)
        //    outColor += DoSpotlight(i, normal, FragPos, viewDir);
    }

    vec3 ambient = vec3(texture(material.diffuseMap, TexCoord)) * material.ambient;
    fragment_color = vec4(ambient + outColor,1.0f);
}

vec3 DoDirectionalLight(int lightIndex, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-LightArray[lightIndex].direction);

    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, TexCoord)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (material.hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, TexCoord))) * spec * material.smoothness;
    }

    // Combine results
    return (diffuse + specular);
}

vec3 DoPointLight(int lightIndex, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(LightArray[lightIndex].position - fragPos);
    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, TexCoord)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (material.hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, TexCoord))) * spec * material.smoothness;
    }

    // Attenuation
    float distance = length(LightArray[lightIndex].position - fragPos);
    vec3 att = LightArray[lightIndex].attenuation;
    float attenuation = 1.0f / (att.x + att.y * distance + att.z * distance * distance);

    // Combine results and add attenuation
    return (diffuse + specular) * attenuation;
}

vec3 DoSpotlight(int lightIndex, vec3 norm, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(LightArray[lightIndex].position - fragPos);
    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, TexCoord)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (material.hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        vec3 specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, TexCoord))) * spec * material.smoothness;
    }

    // Attenuation
    float distance = length(LightArray[lightIndex].position - fragPos);
    vec3 att = LightArray[lightIndex].attenuation;
    float attenuation = 1.0f / (att.x + att.y * distance + att.z * distance * distance);

    // Spotlight intensity
    float cutOff = LightArray[lightIndex].angle - LightArray[lightIndex].angle / 5.0f;
    float outerCutOff = LightArray[lightIndex].angle + LightArray[lightIndex].angle / 5.0f;
    float theta = dot(lightDir, normalize(-LightArray[lightIndex].direction));
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results and add attenuation
    return (diffuse + specular) * attenuation;
}
