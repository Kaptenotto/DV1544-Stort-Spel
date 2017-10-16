#include "ShaderConstants.hlsli"

#define BLOCK_SIZE 16.f
#define NUM_LIGHTS 8

//used by VS & PS
cbuffer Camera : register(b0)
{
    float4x4 ViewProjection;
    float4x4 InvProjection;
    float4x4 View;
    float4 camPos;
}


//used by PS
cbuffer LightBuffer : register(b1)
{
    float4 dirLightPos;
    float3 dirLightColor;
    float dirFade;
}

//Used by PS
cbuffer BulletTimeTimer : register(b2)
{
    float bulletTimer;
};

//Used by PS
struct Light
{
    float4 positionVS;
    float3 positionWS;
    float range;
    float3 color;
    float intensity;
};

StructuredBuffer<uint> LightIndexList : register(t0);
Texture2D<uint2> LightGrid : register(t1);
StructuredBuffer<Light> Lights : register(t2);

Texture2D shadowMap : register(t3);
SamplerState Sampler : register(s0);

SamplerComparisonState cmpSampler : register(s1);

Texture2D diffuseMap : register(t10);
Texture2D normalMap : register(t11);
Texture2D specularMap : register(t12);
Texture2D glowMap : register(t13);

//Returns the shadow amount of a given position
float getShadowValue(float3 lightPos, int sampleCount = 1)
{
    lightPos.x = (lightPos.x * 0.5f) + 0.5f;
    lightPos.y = (lightPos.y * -0.5f) + 0.5f;

    float addedShadow = 0;

    for (int y = -sampleCount; y <= sampleCount; y += 1)
    {
        for (int x = -sampleCount; x <= sampleCount; x += 1)
        {
            addedShadow += shadowMap.SampleCmp(cmpSampler, lightPos.xy, lightPos.z, int2(x, y)).r;
        }
    }

    float shadow = addedShadow / pow(sampleCount * 2 + 1, 2);

    return shadow;
}

//This is because i waas bored, it is not something we will use maybe i think
float3 toonify(float3 color, float intensity)
{
    if (intensity > 0.95)
        color = color; //Yes.
    else if (intensity > 0.5)
        color = 0.7 * color;
    else if (intensity > 0.05)
        color = 0.35 * color;
    else
        color = 0.1 * color;

    return color;
}

float3 getNormalMappedNormal(float3 tangent, float3 biTangent, float3 normal, float2 uv)
{
    float3 normalSample = normalMap.Sample(Sampler, uv);

        //To make sure the tangent is perpendicular
    tangent = normalize(tangent - dot(tangent, normalSample) * normalSample);

    float3x3 tangentMatrix = float3x3(tangent, biTangent, normal);
    normalSample = normalize(normalSample * 2.0 - 1);


    return normalize(mul(normalSample, tangentMatrix));
}

float3 calculateLight(float3 wPos, float3 lightPos, float3 NDCPos, float2 uv, float3 normal)
{
    uint2 tile = uint2(floor(NDCPos.xy / BLOCK_SIZE));
    uint offset = LightGrid[tile].x;
    uint count = LightGrid[tile].y;

    float3 colorSample = diffuseMap.Sample(Sampler, uv);
    float3 specularSample = specularMap.Sample(Sampler, uv);

    ///////////////////////////////DIRECTIONAL LIGHT///////////////////////////////////////
    float3 lightDir = normalize(camPos.xyz - dirLightPos.xyz);
    float diffuseFactor = saturate(dot(normal, normalize(-lightDir)));
    float3 directionalDiffuse = diffuseFactor * dirLightColor;

    float3 posToLightDir = dirLightPos.xyz - wPos.xyz;
    float3 reflectThingDir = normalize(posToLightDir + (camPos.xyz - wPos.xyz));
    float3 directionalSpecularity = pow(saturate(dot(normal, reflectThingDir)), 500) * dirLightColor;

    
    float shadow = getShadowValue(lightPos, 2);

    directionalDiffuse *= dirFade * shadow;
    directionalSpecularity *= dirFade * shadow;

    /////////////////////////////DIRECTIONAL LIGHT END///////////////////////////////////////


    ////////////////////////////////POINT LIGHTS//////////////////////////////////////////////


    float3 pointDiffuse = 0;
    float3 pointSpecular = 0;

    for (uint i = 0; i < count; i++)
    {
        uint idx = LightIndexList[offset + i];
        Light light = Lights[idx];

        float3 posToLight = light.positionWS - wPos.xyz;
        float3 reflectThing = normalize(posToLight + (camPos.xyz - wPos.xyz));

        float distance = length(posToLight);
        float3 normalizedLight = posToLight / distance;
        float attenuation = 1.0f - smoothstep(0, light.range, distance);

        pointDiffuse += saturate(dot(normal, posToLight)) * light.color * attenuation;

        pointSpecular += pow(saturate(dot(normal, reflectThing)), 1000) * light.color * attenuation;
    }
    //////////////////////////////POINT LIGHTS END//////////////////////////////////////////////

    float3 finalDiffuse = saturate(pointDiffuse + directionalDiffuse) * colorSample;
    float3 finalSpecular = saturate(pointSpecular + pointDiffuse) * specularSample;
    
    float3 ambient = float3(0.2, 0.2, 0.3) * colorSample;

    float3 lighting = saturate(finalDiffuse + finalSpecular + ambient);
    
    lighting = adjustSaturation(lighting, bulletTimer);
    lighting = adjustContrast(lighting, 2 - bulletTimer, 0.3);

    return lighting;
}