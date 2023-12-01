#version 450 core
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gOthers;
uniform vec3 viewPos;
uniform float time;
uniform mat4 view;

out vec4 FragColor;

in vec2 TexCoords;

struct Light {
    vec3 position;
    vec3 color;
    // Add other light properties as needed
};

struct Cluster {
    int[256] lightsIndex;
    int lightsCount;
    //Light[64] lights;
};

layout (std430, binding = 0) buffer LightsBuffer {
    Light lights[];
};

layout (std430, binding = 1) buffer ClusterBuffer {
    Cluster[] clusters;
};

layout (std430, binding = 3) buffer SizesBuffer {
    int[] sizes;
    //int[] sizes;
};

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
    return round(value / multiple) * multiple;
}

#define MAX_POINT_LIGHT_PER_TILE 256

void main()
{  
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gDiffuse, TexCoords).xyz;
    float Specular = texture(gOthers, TexCoords).x;
    float metalness = texture(gOthers, TexCoords).y;
    float roughness = texture(gOthers, TexCoords).z;
    vec3 spe = texture(gOthers, TexCoords).xyz;
    
    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    float radius = 2.01f;
    vec2 clusterCount = round(screenSize / clusterSize.xy);
    vec4 viewSpace = view * vec4(FragPos, 1.0);
    vec2 viewSpaceCoords = viewSpace.xy / viewSpace.w;
    vec2 currentClusterPosition = vec2(round(roundToMultiple(TexCoords.x * screenSize.x, clusterSize.x) / clusterSize.x), round(roundToMultiple(TexCoords.y * screenSize.y, clusterSize.y) / clusterSize.y));
    int clusterIndex = int((round(currentClusterPosition.y * (clusterCount.x)) + (currentClusterPosition.x)));

    Cluster currentCluster = clusters[clusterIndex];

    /*
                vec4 viewSpace = view * vec4(lights[i].position.xyz, 1.0);
            vec2 viewSpaceCoords = viewSpace.xy / viewSpace.w;
            int clusterIndex = int((viewSpaceCoords.x * screenSize.x / clusterSize.x) * (viewSpaceCoords.y * screenSize.y / clusterSize.y));
    */
    for (int i = 0; i < MAX_POINT_LIGHT_PER_TILE && currentCluster.lightsIndex[i] != -1; ++i)
    {
        Light light = lights[currentCluster.lightsIndex[i]];
        //light.color = vec3(1.0f, 0.0f, 0.0f);
        vec3 lightPosition = light.position;

        float angle = time * 0.1f;
        mat2 rotationMatrix = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
        vec2 rotatedXZ = rotationMatrix * lightPosition.xz;

        //lightPosition.x = rotatedXZ.x;
        lightPosition.y = 0;
        //lightPosition.z = rotatedXZ.y;

        // calculate distance between light source and current fragment
        float distance = length(lightPosition - FragPos);
        //if(distance < radius * radius)
        if(1 < 2)
        {
            // diffuse
            vec3 lightDir = normalize(lightPosition - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.color * 20;
            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = light.color * 20 * spec * Specular;
            // attenuation
            float constant = 1.0f;
            float quadratic = 0.032f;
            float linear = 0.09f;
            float attenuation = 1.0 / (constant + linear * distance + 
    		    quadratic * (distance * distance));    
            attenuation *= radius;
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += (diffuse + specular) * attenuation;
        }
     }    
    lighting += 1.0f;
    
    FragColor = vec4(Diffuse * lighting, 1.0f);
    //FragColor = vec4(currentCluster.lightsCount / MAX_POINT_LIGHT_PER_TILE, 0.0f, 0.0f, 1.0f);
    //FragColor = vec4(int(currentClusterPosition.x) % 2 == 0 && int(currentClusterPosition.y) % 2 == 0 ? 1.0f : 0.0f, 0.0f, 0.0f, 1.0f);
    //FragColor = vec4(currentClusterPosition.x, currentClusterPosition.y, clusterIndex, 1.0f);
    //FragColor = vec4(clusterColorDebugg, clusterIndex);
    //FragColor = vec4(clusterIndex, clusterIndex, clusterIndex, 1.0f);


    //FragColor = vec4(0.2f) + vec4(mod(viewSpaceCoords.x, clusterSize.x), mod(viewSpaceCoords.y, clusterSize.y), 0.0f, 1.0);
    //FragColor = vec4(attenuation, 1.0f);//vec4(Diffuse, 1.0);
    //FragColor = vec4(pow(Diffuse, vec3(2.2f)) * vec3(1 + texture(lightTexture, TexCoords)), 1.0);
}