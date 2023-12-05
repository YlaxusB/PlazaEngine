#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
uniform mat4 view;

out vec2 TexCoords;

struct Light {
    vec3 position;
    vec3 color;
    // Add other light properties as needed
};
struct Cluster{
    int[256] lightsIndex;
    int lightsCount;
    vec3 minBounds;
    vec3 maxBounds;
};

layout (std430, binding = 0) buffer LightsBuffer {
    Light lights[];
};


layout (std430, binding = 1) buffer ClusterBuffer {
    Cluster[] clusters;
    //int[] sizes;
};
layout (std430, binding = 3) buffer SizesBuffer {
    int[] sizes;
    //int[] sizes;
};

vec2 screenSize = vec2(1820, 720);

vec3 clusterSize = vec3(32, 32, 32);

int lightCount = 0;
void main()
{


    vec2 clusterCount =  floor(screenSize / clusterSize.xy);
        for (int i = 0; i < lights.length(); ++i)
        {
            //Light light = lights[i];
            //vec4 viewSpace = view * vec4(light.position, 1.0);
            //vec2 viewSpaceCoords = viewSpace.xy / viewSpace.w;
            //const int clusterIndex = int(mod(viewSpaceCoords.x, clusterSize.x) * mod(viewSpaceCoords.y, clusterSize.y));
            //clusters[clusterIndex].lightsIndex[lightCount] = lightCount;
            //sizes[i] = 14;
            //lightCount++;
            //Clusters[cluster].lights[0] = light;
            //Clusters[cluster].lightCount++;
        }
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}