//DIFFUSE TEXTURE FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

uniform vec4 color;

uniform int lightCount;
uniform vec3 viewPos;
uniform sampler2D depthMap;
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
    float tiling;
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
in vec4 FragPosLightSpace;
in vec2 TexCoord;
in vec3 Normal;
in mat3 TBN;

out vec4 fragment_color;

float ShadowCalculation(vec4 fragPos, vec3 norm, vec3 lightDir);
vec3 DoDirectionalLight(int lightIndex, vec3 norm, vec3 viewDir, vec2 tiledTexCoord);
vec3 DoPointLight(int lightIndex, vec3 norm, vec3 viewDir, vec2 tiledTexCoord);
//vec3 DoSpotlight(int lightIndex, vec3 norm, vec3 viewDir);

void main( void )
{
    vec3 normal = normalize(Normal);
    vec2 tiledTexCoord = TexCoord * material.tiling;

    if (hasNormalMap) {
        normal = texture(material.normalMap, tiledTexCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
        normal = normalize(TBN * normal);
    }

    vec3 viewDir = vec3(0,0,0);
    if (hasSpecMap)
        viewDir = normalize(viewPos - FragPos);

	vec3 outColor = vec3(0,0,0);

    //light handling
    for(int i=0;i<lightCount;++i)
    {
        if(LightArray[i].type==0)
            outColor += DoDirectionalLight(i, normal, viewDir, tiledTexCoord);
        else if(LightArray[i].type==1)
            outColor += DoPointLight(i, normal, viewDir, tiledTexCoord);
        //else if(LightArray[i].type==2)
        //    outColor += DoSpotlight(i, normal, viewDir);
    }

    vec3 ambient = vec3(texture(material.diffuseMap, tiledTexCoord)) * material.ambient * color.rgb;
    fragment_color = vec4(ambient + outColor,color.w);
}

vec4 blur13(sampler2D image, vec2 uv, vec2 resolution, vec2 direction)
{
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.411764705882353) * direction;
  vec2 off2 = vec2(3.2941176470588234) * direction;
  vec2 off3 = vec2(5.176470588235294) * direction;
  color += texture2D(image, uv) * 0.1964825501511404;
  color += texture2D(image, uv + (off1 / resolution)) * 0.2969069646728344;
  color += texture2D(image, uv - (off1 / resolution)) * 0.2969069646728344;
  color += texture2D(image, uv + (off2 / resolution)) * 0.09447039785044732;
  color += texture2D(image, uv - (off2 / resolution)) * 0.09447039785044732;
  color += texture2D(image, uv + (off3 / resolution)) * 0.010381362401148057;
  color += texture2D(image, uv - (off3 / resolution)) * 0.010381362401148057;
  return color;
}

float ShadowCalculation(vec4 fragPos, vec3 norm, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPos.xyz / fragPos.w; //???
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(depthMap, vec2(projCoords.x,projCoords.y)).r;
    //test
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Check whether current frag pos is in shadow
    float bias = max(0.00001 * (1.0 - dot(norm, lightDir)), 0.00001);
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    if(shadow > 0.75)
    {
        shadow = 0.0;
        vec2 texelSize = 1.0 / textureSize(depthMap, 0);
        for(float x = -1.0; x <= 1.0; ++x)
        {
            for(float y = -1.0; y <= 1.0; ++y)
            {
                float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
                float test = currentDepth;// * 0.95;
                shadow += test - bias > pcfDepth  ? 1.0 : 0.0;

                //float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize * 4);
                //shadow += pcfDepth;
            }
        }
        shadow /= 9.0;
    }


    // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return 1.0 - shadow;
}

vec3 DoDirectionalLight(int lightIndex, vec3 norm, vec3 viewDir, vec2 tiledTexCoord)
{
    vec3 lightDir = normalize(-LightArray[lightIndex].direction);

    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, tiledTexCoord)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        specular = LightArray[lightIndex].color * texture(material.specularMap, TexCoord).r * spec * material.smoothness;
    }

    //float shadow = 1.0f;

    //if(diff>0.001)
    float shadow = ShadowCalculation(FragPosLightSpace, norm, lightDir);

    // Combine results
    return (diffuse + specular) * shadow;
}

vec3 DoPointLight(int lightIndex, vec3 norm, vec3 viewDir, vec2 tiledTexCoord)
{
    vec3 lightDir = normalize(LightArray[lightIndex].position - FragPos);
    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, tiledTexCoord)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        specular = LightArray[lightIndex].color * texture(material.specularMap, tiledTexCoord).r * spec * material.smoothness;
    }

    // Attenuation
    float distance = length(LightArray[lightIndex].position - FragPos);
    vec3 att = LightArray[lightIndex].attenuation;
    float attenuation = 1.0f / (att.x + att.y * distance + att.z * distance * distance);

    // Combine results and add attenuation
    return (diffuse + specular) * attenuation;
}

/*
vec3 DoSpotlight(int lightIndex, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(LightArray[lightIndex].position - FragPos);
    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, TexCoord)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        vec3 specular = (LightArray[lightIndex].color + vec3(texture(material.specularMap, TexCoord))) * spec * material.smoothness;
    }

    // Attenuation
    float distance = length(LightArray[lightIndex].position - FragPos);
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
}*/
