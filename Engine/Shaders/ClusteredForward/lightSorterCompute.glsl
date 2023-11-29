#version 430 core

struct Light{
    vec4 position;
    vec4 color;
};

layout(std430, binding = 0) buffer LightsArray {
    Light lights[];
};

struct Cluster{
    int[2048] lightsIndex;
    int lightsCount;
};

layout(std430, binding = 1) buffer ClusterBuffer {
    Cluster clusters[];
};
layout (location = 3) uniform mat4 view;  
//uniform mat4 view;

vec2 screenSize = vec2(1820, 720);

vec2 clusterSize = vec2(32, 32);
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main()
{
        // Get the global ID of the current invocation
    uint gid = gl_GlobalInvocationID.x;
    for(int clusterInd = 0; clusterInd < 32; clusterInd++)
    {
        for(int i = 0; i < 2000; i++)
        {
            Light light = lights[i];
            vec4 viewSpace = view * vec4(lights[i].position.xyz, 1.0);
            vec2 viewSpaceCoords = viewSpace.xy / viewSpace.w;
            int clusterIndex = int((viewSpaceCoords.x * screenSize.x / clusterSize.x) * (viewSpaceCoords.y * screenSize.y / clusterSize.y));
            Cluster currentCluster = clusters[clusterIndex];
            clusters[clusterIndex].lightsCount += 1;
            clusters[clusterIndex].lightsIndex[currentCluster.lightsCount] = i;
        }
    }
        vec4 viewSpace = view * vec4(vec3(110.0f, 30.0f, 0.0f), 1.0);
        vec2 viewSpaceCoords = viewSpace.xy / viewSpace.w;
clusters[0].lightsIndex[0] = int(lights[0].position.x + lights[0].position.y + lights[0].position.z);//int((viewSpaceCoords.x / clusterSize.x) * (viewSpaceCoords.y / clusterSize.y));
}