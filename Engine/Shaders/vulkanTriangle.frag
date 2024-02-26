#version 460 core
//#extension GL_EXT_descriptor_indexing : enable
#extension GL_EXT_nonuniform_qualifier : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 9) uniform sampler2DArray shadowsDepthMap;
layout(binding = 20) uniform sampler2D textures[];

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



layout(binding = 0) uniform UniformBufferObject {
    mat4 projection;
    mat4 view;
    mat4 model;
    int cascadeCount;
    float farPlane;
    float nearPlane;
    vec4 lightDirection;
    vec4 viewPos;
    mat4 lightSpaceMatrices[16];
    vec4 cascadePlaneDistances[16];
    bool showCascadeLevels;
} ubo;

layout(location = 11) in vec4 FragPos;
layout(location = 12) in vec4 Normal;
layout(location = 13) in vec2 TexCoords;
layout(location = 14) in vec4 TangentLightPos;
layout(location = 15) in vec4 TangentViewPos;
layout(location = 16) in vec4 TangentFragPos;
layout(location = 17) in vec4 worldPos;

const mat4 biasMat = mat4( 
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 1.0, 0.0,
  0.5, 0.5, 0.0, 1.0 );

float textureProj(vec4 shadowCoord, vec2 offset, uint cascadeIndex)
{
	float shadow = 1.0;
	float bias = 0.005;

	if ( shadowCoord.z > -1.0 && shadowCoord.z < 1.0 ) {
		float dist = texture(shadowsDepthMap, vec3(shadowCoord.st + offset, cascadeIndex)).r;
		if (shadowCoord.w > 0 && dist < shadowCoord.z - bias) {
			shadow = 0.1f;
		}
	}
	return shadow;

}


vec3 ShadowCalculation(vec3 fragPosWorldSpace)
{
    // select cascade layer
    vec4 fragPosViewSpace = ubo.view * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);
    int layer = -1;
    for (int i = 0; i < ubo.cascadeCount - 1; ++i)
    {
        if (depthValue < ubo.cascadePlaneDistances[i].x)
        {
            layer = i;
            break;//i = ubo.cascadeCount;
        }
    }
    if (layer == -1)
    {
        layer = ubo.cascadeCount - 1;
        return vec3(0.5f, 0.3f, 1.0f);
    }



    vec4 fragPosLightSpace = (ubo.lightSpaceMatrices[layer]) * vec4(fragPosWorldSpace.xyz, 1.0f);
    // perform perspective divide
    vec4 projCoords = fragPosLightSpace / fragPosLightSpace.w;
    float currentDepth = projCoords.z;
    projCoords = projCoords * 0.5 + 0.5;
    projCoords.y = 1 - projCoords.y;
    //projCoords.y = (1.0f) - projCoords.y;
    //projCoords.y = 1.0f - projCoords.y;

    float shadowFromDepthMap = texture( shadowsDepthMap, vec3(projCoords.xy, float(layer))).r;

    if(ubo.showCascadeLevels)
    return vec3(projCoords.x, layer, shadowFromDepthMap);

    if(ubo.showCascadeLevels){
        if(layer == 0)
            return vec3(1.0f, 0.0f, 0.0f);
        if(layer == 1)
            return vec3(0.0f, 1.0f, 0.0f);
        if(layer == 1)
            return vec3(0.0f, 0.0f, 1.0f);
        if(layer == 2)
            return vec3(1.0f, 1.0f, 0.0f);
        if(layer == 3)
            return vec3(0.0f, 1.0f, 1.0f);
        if(layer == 4)
            return vec3(1.0f, 0.0f, 1.0f);
        if(layer > 4)
            return vec3(0.5f, 0.5f, 0.5f);
    }

   //if( shadowFromDepthMap < currentDepth - 0.0001 )
   //{
   //    return vec3(1.0f);
   //}
   //return vec3(0.0f);

    // transform to [0,1] range
    //projCoords.y = 1 - projCoords.y;
    //projCoords.y = 1 - projCoords.y;
    //projCoords = vec3(projCoords.x, 1.0 - projCoords.y, projCoords.z);
    //return projCoords;
    // get depth of current fragment from light's perspective
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return vec3(0.0f, 1.0f, 1.0f);
    }
    
    if (currentDepth < 0.0)
    {
        return vec3(1.0f, 0.0f, 1.0f);
    }
    // calculate bias (based on depth map resolution and slope)
    vec3 normale;
    bool usingNormal = false;
    if(usingNormal){
        normale = texture(textures[pushConstants.normalIndex], TexCoords).rgb;
        normale = normalize(normale * 2.0 - 1.0);  // this normal is in tangent space
    } else {
        normale = normalize(Normal.xyz);
    }
    float bias = max(0.0005 * (1.0 - dot(normale, ubo.lightDirection.xyz)), 0.00005);
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
    //shadow = texture(shadowsDepthMap, vec3(projCoords.xy, layer)).r;///= totalTexels;
    float pcfDepth = texture(shadowsDepthMap, vec3(projCoords.xy, layer)).r;
    shadow /= totalTexels; //texture(shadowsDepthMap, vec3(projCoords.xy, layer)).r;//= (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
    return vec3(shadow);
    //return vec3(texture(shadowsDepthMap, vec3(projCoords.xy, layer)).r);//vec3(shadow);
    if(layer == 0)
        return vec3(1.0f, 0.0f, 0.0f);
    if(layer == 1)
        return vec3(0.0f, 1.0f, 0.0f);
    if(layer == 1)
        return vec3(0.0f, 0.0f, 1.0f);
    if(layer == 2)
        return vec3(1.0f, 1.0f, 0.0f);
    if(layer == 3)
        return vec3(0.0f, 1.0f, 1.0f);
    if(layer == 4)
        return vec3(1.0f, 0.0f, 1.0f);
    if(layer > 4)
        return vec3(0.5f, 0.5f, 0.5f);

    //shadow = texture(shadowsDepthMap, vec3(projCoords.xy, layer)).r;
   // return shadow;

   // float depth = texture(shadowsDepthMap, vec3(shadowCoord.xy, layer)).r;

  //float biase = 0.0005;
  //return shadowCoord.z > depth + biase ? vec3(1.0f) : vec3(0.0f);

   //return vec3(shadow);

    //if(shadowCoord.x > 1){
    //    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    //}
    //    if(shadowCoord.y > 1){
    //    outColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    //}
    //    if(shadowCoord.x < 0){
    //    outColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    //}
    //    if(shadowCoord.x < 0){
    //    outColor = vec4(0.0f, 1.0f, 1.0f, 1.0f);
    //}
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

        vec4 fragPosViewSpace = ubo.view * vec4(FragPos.xyz, 1.0);
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
        layer = ubo.cascadeCount - 1;
    }

    int ind = 4;
    vec4 shadowCoord = (ubo.lightSpaceMatrices[layer]) * vec4(FragPos.xyz, 1.0);	
    shadowCoord = shadowCoord / shadowCoord.w;
    shadowCoord = shadowCoord * 0.5 + 0.5;

    //outColor = vec4(vec3(textureProj(shadowCoord / shadowCoord.w, vec2(0.0), ind)), 1.0f);
    if(!ubo.showCascadeLevels)
  outColor *= vec4((vec3(1.0f) - ShadowCalculation(FragPos.xyz) + 0.25f).xyz, 1.0f);
  else
  outColor = vec4(vec3(ShadowCalculation(FragPos.xyz)), 1.0f);
    //outColor *= vec4(vec3(textureProj(shadowCoord, vec2(0.0), layer)), 1.0f);

    //outColor = vec4(vec3(textureProj(shadowCoord, vec2(0.0), ind)), 1.0f);
    //outColor = vec4(FragPos.xyz / vec3(10.0f), 1.0f);
//   outColor = vec4(vec3(ShadowCalculation(FragPos.xyz)), 1.0f);
}