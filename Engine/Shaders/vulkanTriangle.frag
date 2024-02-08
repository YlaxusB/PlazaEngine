#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 9) uniform sampler2DArray shadowsDepthMap;
layout(binding = 10) uniform sampler2D textures[];

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in mat4 model;

layout(push_constant) uniform PushConstants {
    vec4 color;
    float intensity;
    int diffuseIndex;
    int normalIndex;
} pushConstants;

layout(location = 0) out vec4 outColor;



layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 projection;
    mat4 view;
    mat4 model;
    int cascadeCount;
    float farPlane;
    float nearPlane;
    vec4 lightDirection;
    vec4 viewPos;
    mat4[16] lightSpaceMatrices;
    vec4[16] cascadePlaneDistances;
} ubo;

layout(location = 11) in vec4 FragPos;
layout(location = 12) in vec4 Normal;
layout(location = 13) in vec2 TexCoords;
layout(location = 14) in vec4 TangentLightPos;
layout(location = 15) in vec4 TangentViewPos;
layout(location = 16) in vec4 TangentFragPos;
layout(location = 17) in vec4 worldPos;


float ShadowCalculation(vec3 fragPosWorldSpace)
{
    // select cascade layer
    vec4 fragPosViewSpace = ubo.view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);
    int layer = -1;
    for (int i = 0; i < ubo.cascadeCount; ++i)
    {
        if (depthValue < ubo.cascadePlaneDistances[i].x)
        {
            layer = i;
            break;//i = ubo.cascadeCount;
        }
    }
    if (layer == -1)
    {
        layer = ubo.cascadeCount;
    }

    vec4 fragPosLightSpace = ubo.lightSpaceMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    projCoords = vec3(projCoords.x, 1.0 - projCoords.y, projCoords.z);

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        //return 0.0;
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normal;
    bool usingNormal = false;
    if(usingNormal){
        normal = texture(textures[pushConstants.normalIndex], TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    } else {
        normal = normalize(Normal.xyz);
    }
    float bias = max(0.0005 * (1.0 - dot(normal, ubo.lightDirection.xyz)), 0.00005);
    const float biasModifier = 0.5f;
    if (layer == ubo.cascadeCount)
    {
        bias *= 1 / (ubo.farPlane * biasModifier);
    }
    else
    {
        bias *= 1 / ((ubo.cascadePlaneDistances[layer].x) * biasModifier);
    }
    float distanceToCamera = distance(ubo.viewPos.xyz, projCoords.xyz);
    
    bias = 0.0001;
    float floatVal = 3 - (texture(shadowsDepthMap, vec3(projCoords.xyz)).r * 2.3);
    int pcfCount = 5;// + int(floatVal);
    float mapSize = 4096.0 * 4;
    float texelSize = (1.0 / mapSize * 2) * floatVal;
    float total = 0.0;
    float totalTexels = (pcfCount * 2.0 + 1.0) * (pcfCount * 2.0 + 1.0);
    // PCF
    float shadow = 0.0;
    //vec2 texelSize = 1.0 / vec2(textureSize(shadowsDepthMap, 0));
    for(int x = -pcfCount; x <= pcfCount; ++x)
    {
        for(int y = -pcfCount; y <= pcfCount; ++y)
        {
            float pcfDepth = texture(shadowsDepthMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= totalTexels;

    //if(layer == 0)
    //    return vec3(1.0f, 0.0f, 0.0f);
    //if(layer == 1)
    //    return vec3(0.0f, 1.0f, 0.0f);
    //if(layer == 1)
    //    return vec3(0.0f, 0.0f, 1.0f);
    //if(layer == 2)
    //    return vec3(1.0f, 1.0f, 0.0f);
    //if(layer == 3)
    //    return vec3(0.0f, 1.0f, 1.0f);
    //if(layer == 4)
    //    return vec3(1.0f, 0.0f, 1.0f);
    //if(layer > 4)
    //    return vec3(0.5f, 0.5f, 0.5f);

    //shadow = texture(shadowsDepthMap, vec3(projCoords.xy, layer)).r;
    return shadow;
}

void main() {
    if(pushConstants.diffuseIndex > -1)
    {
        outColor = texture(textures[pushConstants.diffuseIndex], fragTexCoord);;
    }
    else
    {
        outColor = vec4(pushConstants.color.xyz, 1.0f);
    }
    outColor *= vec4((vec3(1.0f) - ShadowCalculation(FragPos.xyz)).xyz, 1.0f);
}