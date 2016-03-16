//DIFFUSE TEXTURE FRAGMENT SHADER
#version 330 // for glsl version (12 is for older versions , say opengl 2.1
#define MGE_MAX_LIGHTS 8

uniform vec4 color;

uniform int lightCount;
uniform vec3 viewPos;
uniform sampler2D depthMapNear;
uniform sampler2D depthMapFar;
uniform sampler2D depthMapMid;
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
    float tilingDiffuse;
    float tilingNormal;
    float tilingSpecular;
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
in vec4 FragPosLightSpaceNear;
in vec4 FragPosLightSpaceFar;
in vec4 FragPosLightSpaceMid;
in vec2 TexCoord;
in vec3 Normal;
in mat3 TBN;
in float camSpaceDepth;

out vec4 fragment_color;

float ShadowCalculation(vec4 fragPos, vec3 norm, vec3 lightDir);
vec3 DoDirectionalLight(int lightIndex, vec3 norm, vec3 viewDir);
vec3 DoPointLight(int lightIndex, vec3 norm, vec3 viewDir);
//vec3 DoSpotlight(int lightIndex, vec3 norm, vec3 viewDir);

vec2 tiledTexCoordDiffuse;
vec2 tiledTexCoordSpecular;

float BilinearShadowSample(sampler2D samp,vec2 uv, float texSize)
{
    //float tSize = 4096;//assuming square, 4096
    float iSize = 1.0 / texSize;
    vec2 scaledUV = uv * texSize;

    vec2 floors = vec2(floor(scaledUV.x),floor(scaledUV.y));
    vec2 fracts = vec2(fract(scaledUV.x),fract(scaledUV.y));


    float bottom = mix(texture(samp, floors              * iSize).r,texture(samp,(floors + vec2(1,0)) * iSize).r,fracts.x);
    float top    = mix(texture(samp,(floors + vec2(0,1)) * iSize).r,texture(samp,(floors + vec2(1,1)) * iSize).r,fracts.x);

    return mix(bottom,top,fracts.y);
}

void main( void )
{
    vec3 normal = normalize(Normal);

    tiledTexCoordDiffuse = TexCoord * material.tilingDiffuse;
    tiledTexCoordSpecular = TexCoord * material.tilingDiffuse;

    if (hasNormalMap) {
        normal = texture(material.normalMap, TexCoord * material.tilingNormal).rgb;
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
            outColor += DoDirectionalLight(i, normal, viewDir);
        else if(LightArray[i].type==1)
            outColor += DoPointLight(i, normal, viewDir);
        //else if(LightArray[i].type==2)
        //    outColor += DoSpotlight(i, normal, viewDir);
    }

    vec3 ambient = vec3(texture(material.diffuseMap, tiledTexCoordDiffuse)) * material.ambient * color.rgb;
    fragment_color = vec4(ambient + outColor,color.w);

    //fog
    float fogBlend = (2000.0 + camSpaceDepth) * 0.0005002501; //(fogFar - fragDepth) / (fogFar - fogNear)
    fogBlend = clamp(fogBlend,0.5,1.0);

    fragment_color = fragment_color * fogBlend + (1.0 - fogBlend) * vec4(0.5,0.5,0.5,1.0);

}

bool CheckFrustum(vec2 coords)
{
    if(coords.x < -0.998 || coords.x > 0.998 || coords.y < -0.998 || coords.y > 0.998)
        return false;

    return true;
}

float ShadowCalculation(vec3 norm, vec3 lightDir)
{


    //first use the NEAR map to see if a shadow is formed. the NEAR shadows override the far ones

    vec3 texSizes = vec3(4096,4096,4096);//near,mid,far

    // perform perspective divide
    //vec3 projCoords = FragPosLightSpaceNear.xyz / FragPosLightSpaceNear.w;
    // Transform to [0,1] range
    vec3 projCoords;// = FragPosLightSpaceNear.xyz * 0.5 + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(depthMapNear, vec2(projCoords.x,projCoords.y)).r;
    float closestDepth;// = BilinearShadowSample(depthMapNear,projCoords.xy,texSizes.x);
    // Get depth of current fragment from light's perspective
    //float currentDepth; = projCoords.z;
    // Check whether current frag pos is in shadow
    float bias = max(0.0001 * (1.0 - dot(norm, lightDir)), 0.0001);
    float shadow;// = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    //projCoords = FragPosLightSpaceNear.xyz * 0.5 + 0.5;
    if(CheckFrustum(FragPosLightSpaceNear.xy))
    {
        projCoords = FragPosLightSpaceNear.xyz * 0.5 + 0.5;
        if(projCoords.z > 1.0001)//if near and mid start "bleeding" shadows, use this with them as well
            return 1.0;

        closestDepth = BilinearShadowSample(depthMapNear,projCoords.xy,texSizes.x);
        //bias = max(0.00001 * (1.0 - dot(norm, lightDir)), 0.00001);
        shadow = projCoords.z - bias > closestDepth  ? 1.0 : 0.0;


        if(shadow > 0.75)
        {
            shadow = 0.0;
            float texelSize = 1.0 / texSizes.x;
            for(float x = -1.0; x <= 1.0; ++x)
            {
                for(float y = -1.0; y <= 1.0; ++y)
                {
                    //float pcfDepth = texture(depthMapNear, projCoords.xy + vec2(x, y) * texelSize).r;
                    float pcfDepth = BilinearShadowSample(depthMapNear,projCoords.xy + vec2(x, y) * texelSize ,texSizes.x);//this actually looks decent, for once
                    shadow += projCoords.z - bias > pcfDepth  ? 1.0 : 0.0;

                }
            }
            shadow /= 9.0;

            return 1.0 - shadow;
        }
    }
    else
    if(CheckFrustum(FragPosLightSpaceMid.xy))
    {
        projCoords = FragPosLightSpaceMid.xyz * 0.5 + 0.5;

        if(projCoords.z > 1.0001)//if near and mid start "bleeding" shadows, use this with them as well
            return 1.0;

        closestDepth = BilinearShadowSample(depthMapMid,projCoords.xy,texSizes.y);
        //bias = max(0.0001 * (1.0 - dot(norm, lightDir)), 0.0001);//0.002 was decent
        shadow = projCoords.z - bias > closestDepth  ? 1.0 : 0.0;

        if(shadow > 0.75)
        {
            shadow = 0.0;
            float texelSize = 1.0 / texSizes.y;
            for(float x = -1.0; x <= 1.0; ++x)
            {
                for(float y = -1.0; y <= 1.0; ++y)
                {
                    float pcfDepth = texture(depthMapMid, projCoords.xy + vec2(x, y) * texelSize).r;
                    //float pcfDepth = BilinearShadowSample(depthMapMid,projCoords.xy + vec2(x, y) * texelSize,texSizes.y);
                    shadow += projCoords.z - bias > pcfDepth  ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;

            return 1.0 - shadow;
        }
    }
    else
    if(CheckFrustum(FragPosLightSpaceFar.xy))
    {
        projCoords = FragPosLightSpaceFar.xyz * 0.5 + 0.5;

        if(projCoords.z > 1.0001)//if near and mid start "bleeding" shadows, use this with them as well
            return 1.0;

        closestDepth = BilinearShadowSample(depthMapFar,projCoords.xy,texSizes.z);
        bias = max(0.00125 * (1.0 - dot(norm, lightDir)), 0.00125);
        shadow = projCoords.z - bias > closestDepth  ? 1.0 : 0.0;

        if(shadow > 0.75) return 0.0;
    }

    return 1.0;

}

vec3 DoDirectionalLight(int lightIndex, vec3 norm, vec3 viewDir)
{
    vec3 lightDir = normalize(-LightArray[lightIndex].direction);

    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, tiledTexCoordDiffuse)) * diff * 1.2;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        specular = LightArray[lightIndex].color * texture(material.specularMap, tiledTexCoordSpecular).r * spec * material.smoothness;
    }

    float shadow = (diff==0.0) ? 0.5f : ShadowCalculation(norm, lightDir) * 0.5f;
    //float shadow = 1.0;

    // Combine results
    return (diffuse + specular) * shadow;
}

vec3 DoPointLight(int lightIndex, vec3 norm, vec3 viewDir)
{
    vec3 diffVector = LightArray[lightIndex].position - FragPos;

    vec3 lightDir = normalize(diffVector);
    // Diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = LightArray[lightIndex].color * vec3(texture(material.diffuseMap, tiledTexCoordDiffuse)) * diff;

    // Specular shading
    float spec = 0.0f;
    vec3 specular = vec3(0,0,0);
    if (hasSpecMap) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
        specular = LightArray[lightIndex].color * texture(material.specularMap, tiledTexCoordSpecular).r * spec * material.smoothness;
    }

    // Attenuation
    float dist = length(diffVector);
    vec3 att = LightArray[lightIndex].attenuation;
    float attenuation = 1.0f / (att.x + att.y * dist + att.z * dist * dist);

    // Combine results and add attenuation

    //vec3 ret = (diffuse + specular) * attenuation;
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
