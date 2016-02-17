//DIFFUSE TEXTURE FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

in vec2 texCoord;
in vec3 worldNormal;
in vec3 position;
out vec4 fragment_color;

uniform int lightCount;
uniform vec3 viewPos;

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

vec3 DoDirectionalLight(int lightIndex, vec3 worldNormal, vec3 viewDir);
vec3 DoPointLight(int lightIndex, vec3 worldNormal, vec3 worldVertex, vec3 viewDir);
vec3 DoSpotlight(int lightIndex, vec3 worldNormal, vec3 worldVertex, vec3 viewDir);

void main( void )
{
    vec3 normal = normalize(worldNormal);
	// Obtain normal from normal map in range [0,1]
    //normal = texture(material.normalMap, texCoord).rgb;
    // Transform normal vector to range [-1,1]
    //normal = normalize(normal * 2.0 - 1.0);

    vec3 viewDir = normalize(viewPos - position);

	vec3 outColor = vec3(0,0,0);

    //light handling
    for(int i=0;i<lightCount;++i)
    {
        if(LightArray[i].type==0)
            outColor += DoDirectionalLight(i,normal, viewDir);

        if(LightArray[i].type==1)
            outColor += DoPointLight(i, normal, position, viewDir);

        if(LightArray[i].type==2)
            outColor += DoSpotlight(i, normal, position, viewDir);
    }

    fragment_color = vec4(outColor,1.0f);
}

vec3 DoDirectionalLight(int lightIndex, vec3 worldNormal, vec3 viewDir)
{
    vec3 lightDir = normalize(-LightArray[lightIndex].direction);
    // Diffuse shading
    float diff = max(dot(worldNormal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, worldNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Add material influence
    vec3 ambient = vec3(texture(material.diffuseMap, texCoord)) * material.ambient;
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, texCoord)) * diff;
    vec3 specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, texCoord))) * spec * material.smoothness;
    // Combine results
    return (ambient + diffuse + specular);
}

vec3 DoPointLight(int lightIndex, vec3 worldNormal, vec3 worldVertex, vec3 viewDir)
{
    vec3 lightDir = normalize(LightArray[lightIndex].position - worldVertex);
    // Diffuse shading
    float diff = max(dot(worldNormal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, worldNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float distance = length(LightArray[lightIndex].position - worldVertex);
    vec3 att = LightArray[lightIndex].attenuation;
    float attenuation = 1.0f / (att.x + att.y * distance + att.z * distance * distance);
    //float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Add material influence
    vec3 ambient = vec3(texture(material.diffuseMap, texCoord)) * material.ambient;
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, texCoord)) * diff;
    vec3 specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, texCoord))) * spec * material.smoothness;
	// Add attenuation
	ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    // Combine results
    return (ambient + diffuse + specular);
}

vec3 DoSpotlight(int lightIndex, vec3 worldNormal, vec3 worldVertex, vec3 viewDir)
{
    vec3 lightDir = normalize(LightArray[lightIndex].position - worldVertex);
    // Diffuse shading
    float diff = max(dot(worldNormal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, worldNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float distance = length(LightArray[lightIndex].position - worldVertex);
    vec3 att = LightArray[lightIndex].attenuation;
    float attenuation = 1.0f / (att.x + att.y * distance + att.z * distance * distance);
    // Spotlight intensity
    float theta = dot(lightDir, normalize(-LightArray[lightIndex].direction)); ///minus dir of fucking plus?!
    //float epsilon = (LightArray[lightIndex].angle / 5f) - LightArray[lightIndex].angle;
    //float intensity = clamp((theta - LightArray[lightIndex].angle) / epsilon, 0.0, 1.0);
    float intensity = 0f;
    if (theta > LightArray[lightIndex].angle)
    intensity =  1f;
    // Combine results
    vec3 ambient = vec3(texture(material.diffuseMap, texCoord)) * material.ambient;
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, texCoord)) * diff;
    vec3 specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, texCoord))) * spec * material.smoothness;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}
