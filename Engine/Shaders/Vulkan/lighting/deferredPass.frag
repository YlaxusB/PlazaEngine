#version 450

//layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 0) uniform sampler2D gNormal;
layout (binding = 1) uniform sampler2D gDiffuse;
layout (binding = 2) uniform sampler2D gOthers;
layout (binding = 3) uniform sampler2D gSceneDepth;

layout(push_constant) uniform PushConstants {
    vec3 viewPos;
    float time;
    mat4 view;
    mat4 projection;
    int lightCount;
    vec4 ambientLightColor;
} pushConstants;

layout(location = 0) out vec4 FragColor;
layout(location = 1) in vec2 TexCoords;

struct LightStruct {
    vec3 color;
    float radius;
    vec3 position;
    float intensity;
    float cutoff;
    float minRadius;
};

struct Cluster{
    int[256] lightsIndex;
    vec3 minBounds;
    int lightsCount;
    vec3 maxBounds;
    int alignment;
};

struct Plane
{
    vec3 Normal;
    float Distance;
};

struct Frustum
{
    Plane planes[4];
};

layout (std430, binding = 4) buffer LightsBuffer {
    LightStruct lights[];
};

layout (std430, binding = 5) buffer ClusterBuffer {
    Cluster[] clusters;
};

//layout (std430, binding = 6) buffer FrustumsBuffer {
//    Frustum frustums[];
//};

float attenuate(vec3 lightDirection, float radius) {
	float cutoff = .99;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (cutoff);

	return clamp(attenuation, 0.0, 1.0);
}

vec2 screenSize = vec2(1820, 720);

vec3 clusterSize = vec3(32, 32, 32);

int roundUp(float numToRound, float multiple)
{
numToRound = round(numToRound);
    if (multiple == 0)
        return int(numToRound);

    int remainder = int(numToRound) % int(multiple);
    if (remainder == 0)
        return int(round(numToRound));

    return int(round(numToRound + multiple - remainder));
}

float roundToMultiple(float value, float multiple) {
    return floor(value / multiple) * multiple;
}

vec2 indexToRowCol(int index, vec2 clusterCount) {
    float col = mod(float(index), clusterCount.x);
    float row = floor(float(index) / clusterCount.x);
    return vec2(row, col);
}

int GetGridIndex(vec2 posXY)
{
    const vec2 pos = vec2(uint(posXY.x), uint(posXY.y));
    const uint tileX = uint(ceil(screenSize.x / clusterSize.x));
    return int((pos.x / clusterSize.x) + (tileX * (pos.y / clusterSize.x)));
}

vec4 HeatMap(int clusterIndex, int numLights)
{
    vec3 color;
    if(numLights <= 0)
    {
        color = vec3(0.5f, 0.5f, 0.5f);    
    }
    else if(numLights < 10)
    {
        color = vec3(0.2f, 1.0f, 0.0f);    
    }
    else if(numLights < 50)
    {
        color = vec3(0.4f, 0.8f, 0.0f);    
    }
    else if(numLights < 100)
    {
        color = vec3(0.8f, 0.4f, 0.0f);    
    }
    else if(numLights >= 100)
    {
        color = vec3(1.0f, 0.0f, 0.0f);    
    }
    return vec4(color, 1.0f);
}

vec2 worldToScreen(vec3 position){
    vec4 viewSpace = pushConstants.projection * (pushConstants.view * vec4(position, 1.0));
    vec3 ndcPosition = viewSpace.xyz / viewSpace.w;
    vec2 uvCoordinates = (ndcPosition.xy + 1.0) / 2.0;
    return uvCoordinates;
}

int GetIndex(vec2 screenPos, vec2 clusterCount)
{
    vec2 currentClusterPosition = vec2(ceil(roundToMultiple(screenPos.x * screenSize.x, clusterSize.x) / clusterSize.x), ceil(roundToMultiple(screenPos.y * screenSize.y, clusterSize.y) / clusterSize.y));
    return int(((currentClusterPosition.y * (clusterCount.x)) + (currentClusterPosition.x)));
}

const float PI = 3.14159265359;

float D_GGX(float dotNH, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
    return (alpha2)/(PI * denom*denom); 
}

float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    float GL = dotNL / (dotNL * (1.0 - k) + k);
    float GV = dotNV / (dotNV * (1.0 - k) + k);
    return GL * GV;
}

vec3 F_Schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 specularContribution(vec3 L, vec3 V, vec3 N, vec3 F0, float metallic, float roughness, vec3 materialColor)
{
    vec3 H = normalize (V + L);
    float dotNH = clamp(dot(N, H), 0.0, 1.0);
    float dotNV = clamp(dot(N, V), 0.0, 1.0);
    float dotNL = clamp(dot(N, L), 0.0, 1.0);

    vec3 color = vec3(0.0);

    if (dotNL > 0.0) {
        float D = D_GGX(dotNH, roughness); 
        float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
        vec3 F = F_Schlick(dotNV, F0);		
        vec3 spec = D * F * G / (4.0 * dotNL * dotNV + 0.001);		
        vec3 kD = (vec3(1.0) - F) * (1.0 - metallic);			
        color += (kD * materialColor / PI + spec) * dotNL;
    }

    return color;
}

#define MAX_POINT_LIGHT_PER_TILE 2048
//#define SHOW_HEATMAP

float linearizeDepth(float depth) {
    float far = 15000.0f;
    float near = 0.01f;
    return (2.0 * near * far) / (far + near - depth * (far - near));
}

void main()
{  
    // retrieve data from gbuffer
    vec3 Diffuse = texture(gDiffuse, TexCoords).xyz;
    // then calculate lighting as usual
    vec3 lighting  = vec3(0.0f);//Diffuse * 0.1; // hard-coded ambient component

    mat4 viewProjection = pushConstants.projection * pushConstants.view;
       float depth = (texture(gSceneDepth, TexCoords).r);
       float ndcZ = (depth) * 2.0f - 1.0f; 
        vec4 ndcPosition = vec4(gl_FragCoord.xy * (1.0f / screenSize) * 2.0 - 1.0, (depth), 1.0);
        ndcPosition.y *= -1;
        vec4 worldPosition = inverse(viewProjection) * ndcPosition;
        worldPosition.xyz /= worldPosition.w;
      vec3 FragPos = worldPosition.xyz;

      //vec2 inv_resolution = vec2(1.0f / screenSize);
      //vec4 clip = vec4(gl_FragCoord.xy * inv_resolution * 2.0 - 1.0, depth, 1.0);
      //clip.y *= -1;
      //vec4 world_w = inverse(viewProjection) * clip;
      //
      //FragPos.xyz = world_w.xyz / world_w.w;

        // Reconstruct world-space position from depth
       // vec2 screenPos = (gl_FragCoord.xy / vec2(1820.0f, 720.0f)) * 2.0 - 1.0;
       // vec4 clipSpacePos =  vec4(fragCoord.xy * 2.0 - 1.0, depth * 2.0 - 1.0, 1.0);//vec4(screenPos.xy, depth, 1.0);
       // vec4 viewSpacePos = inverse(pushConstants.projection) * clipSpacePos;
       // viewSpacePos /= viewSpacePos.w;
       // vec4 FragPos = (pushConstants.inverseView * viewSpacePos);
       // FragPos = FragPos.xyz / FragPos.w;
        vec2 clusterCount = ceil(screenSize / clusterSize.xy);
        int clusterIndex = GetIndex(TexCoords, clusterCount);
        Cluster currentCluster = clusters[clusterIndex];
    if(pushConstants.lightCount > 0 && depth != 1.0f) {

       // // Reconstruct normal from 2-component packed normal
       // vec2 encodedNormal = texture(gNormal, TexCoords).xy;
       // vec3 Normal;
       // Normal.xy = encodedNormal;
       // Normal.z = sqrt(1.0 - dot(Normal.xy, Normal.xy));
       // Normal = normalize(Normal);

        //vec3 FragPos = texture(gPosition, TexCoords).rgb;
        vec3 Normal = texture(gNormal, TexCoords).rgb;
        const vec3 Others = texture(gOthers, TexCoords).xyz;
        const float Specular = Others.x;//texture(gOthers, TexCoords).x;
        const float metalness = Others.y;//texture(gOthers, TexCoords).y;
        const float roughness = Others.z;//texture(gOthers, TexCoords).z;
        //vec3 spe = texture(gOthers, TexCoords).xyz;


        vec3 viewDir  = normalize(pushConstants.viewPos - FragPos);

        vec3 F0 = vec3(0.04); 
        F0 = mix(F0, Diffuse, metalness);

        vec3 V = normalize(pushConstants.viewPos - FragPos.xyz);

        for (int i = 0; i < MAX_POINT_LIGHT_PER_TILE && clusters[clusterIndex].lightsIndex[i] != -1; ++i) {

            LightStruct light = lights[clusters[clusterIndex].lightsIndex[i]];
            vec3 lightPosition = light.position; //vec3(pushConstants.view * vec4(light.position, 1.0));

            vec3 lightDirection = normalize(lightPosition.xyz - FragPos); // Directional light direction
            float distance = length(lightPosition - FragPos);

            //if(distance < light.radius)
            //{
            
            //vec3 L = lightPosition.xyz - FragPos;
            float dist = length(lightDirection);
            lightDirection = normalize(lightPosition.xyz - FragPos);
            //if(dist < 25)
            //lighting += vec3(0.3f, 1.0f, 0.0f);
            
           float atten = light.radius / (pow(distance, light.cutoff) + 1.0);//light.radius / (pow(dist, light.cutoff) + 1.0);

            vec3 N = normalize(Normal);
            float NdotL = max(0.0, dot(N, lightDirection));
            vec3 lightColor = light.color * light.intensity ;
            vec3 diff = min(lightColor * Diffuse * NdotL * atten, lightColor);
            //float atten = light.radius / (pow(dist, light.cutoff) + 1.0);
            vec3 Lo = lightColor * (specularContribution(lightDirection, V, N, F0, metalness, roughness, Diffuse) * atten);
           lighting += Lo;


           //vec3 Lo = (kD * Diffuse / PI + specular) * radiance * NdotL;
           //lgihting += Lo;

           //lighting += FragPos;//FragPos;//lightColor * atten;
           //}
        }    
     }
    //lighting += 1.0f;
    vec3 color = Diffuse + lighting;

#ifdef SHOW_HEATMAP
    vec3 heatmap = HeatMap(clusterIndex, currentCluster.lightsCount).xyz;
    color = (color + heatmap) * 0.5f;
#endif

    FragColor = vec4(color, 1.0f);
}