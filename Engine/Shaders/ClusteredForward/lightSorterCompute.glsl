#version 430 core

struct Light{
    vec3 position;
    vec3 color;
};

layout(std430, binding = 0) buffer LightsArray {
    Light lights[];
};

struct Cluster{
    int[256] lightsIndex;
    int lightsCount;
};

layout(std430, binding = 1) buffer ClusterBuffer {
    Cluster clusters[];
};
layout (location = 3) uniform mat4 view;  
layout (location = 4) uniform mat4 projection;
layout (location = 5) uniform bool first;
//uniform mat4 view;

vec2 screenSize = vec2(1820, 720);

vec2 clusterSize = vec2(32, 32);

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

int rowColToIndex(vec2 rowCol, vec2 clusterCount) {
    return int((round(rowCol.y * (clusterCount.x)) + (rowCol.x)));
}


layout(std430, binding = 6) buffer OutputBuffer {
    vec3[] result;
};

// world position to screen position between 0 and 1
vec2 worldToScreen(vec3 position){
    vec4 viewSpace = projection * (view * vec4(position, 1.0));
    vec3 ndcPosition = viewSpace.xyz / viewSpace.w;
    vec2 uvCoordinates = (ndcPosition.xy + 1.0) / 2.0;
    return uvCoordinates;
}

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main()
{
    //ivec2 clusterCount = ivec2(screenSize / clusterSize);
    if(first){
    for(int i = 0; i < 1424; i++){
    clusters[i].lightsCount = 0;
    for(int k = 0; k < 64; ++k){
    clusters[i].lightsIndex[k] = -1;
    }
    }
    }
    else{
        // Get the global ID of the current invocation
    int gid = int(gl_GlobalInvocationID.x);
    //for(int j = 0; j < 1424; ++j){
    //for(int i = 0; i < 500; ++i){
    //clusters[j].lightsCount+=1;
    //clusters[j].lightsIndex[clusters[j].lightsCount] = i;
    //}
    //}
            vec2 clusterCount = ceil(screenSize / clusterSize.xy);
        for(int i = 0; i < 20; ++i)
        {
            Light light = lights[i];
            light.position.y = 0;

            vec3 radiusVector = vec3(15.0f);
            vec3 maxPos = light.position.xyz + radiusVector;
            vec3 minPos = light.position.xyz - radiusVector;

            vec2 maxScreenPos = worldToScreen(maxPos);
            vec2 minScreenPos = worldToScreen(minPos);
                        
            vec2 maxClusterPosition = vec2(round(roundToMultiple(maxScreenPos.x * screenSize.x, clusterSize.x) / clusterSize.x), round(roundToMultiple(maxScreenPos .y * screenSize.y, clusterSize.y) / clusterSize.y));
            vec2 minClusterPosition = vec2(round(roundToMultiple(minScreenPos.x * screenSize.x, clusterSize.x) / clusterSize.x), round(roundToMultiple(minScreenPos .y * screenSize.y, clusterSize.y) / clusterSize.y));

            int maxIndex = rowColToIndex(maxClusterPosition, clusterCount);
            int minIndex = rowColToIndex(minClusterPosition, clusterCount);
        
            vec4 viewSpace = projection * view * vec4(light.position.xyz, 1.0);
                vec3 ndcPosition = viewSpace.xyz / viewSpace.w;
        
            // Map NDC to UV coordinates
            vec2 uvCoordinates = 0.5 * (ndcPosition.xy + 1.0);
            vec2 viewSpaceCoords =uvCoordinates; //viewSpace.xy / viewSpace.w;
            vec2 currentClusterPosition = vec2(round(roundToMultiple(viewSpaceCoords.x * screenSize.x, clusterSize.x) / clusterSize.x), round(roundToMultiple(viewSpaceCoords .y * screenSize.y, clusterSize.y) / clusterSize.y));
            int clusterIndex = int((round(currentClusterPosition.y * (clusterCount.x)) + (currentClusterPosition.x)));
        
            Cluster currentCluster = clusters[clusterIndex];
            //clusters[clusterIndex].lightsCount += 1;
            //int c = clusters[clusterIndex].lightsCount;
            //clusters[clusterIndex].lightsIndex[c - 1] = i;

            //for(int j = minIndex; j < maxIndex; ++j){
            //    clusters[j].lightsCount += 1;
            //    int count = clusters[j].lightsCount;
            //    clusters[j].lightsIndex[count - 1] = i;
            //}
        
            int jStarting = clusterIndex > 8 ? -8 : 0;
            int kStarting = clusterIndex / clusterSize.x > 8 ? -8 : 0;
            if(minIndex > 0 && maxIndex < 1500){
            for(int j = -8; j < 8; j++)
            {
                for(int k = -8; k < 8; k++)
                {
                    int offset = rowColToIndex(currentClusterPosition + vec2(j, k), clusterCount);
                    if(clusterIndex + offset > 0)
                    {
                        clusters[offset].lightsCount += 1;
                        int count = clusters[offset].lightsCount;
                        clusters[offset].lightsIndex[count - 1] = i;   
                    }
                }
            }
            result[i] = vec3(uvCoordinates, clusterIndex);
        }
        }
        }
        //vec4 viewSpace = view * vec4(vec3(110.0f, 30.0f, 0.0f), 1.0);
        //vec2 viewSpaceCoords = viewSpace.xy / viewSpace.w;
        //clusters[0].lightsIndex[0] = int(lights[0].position.x + lights[0].position.y + lights[0].position.z);//int((viewSpaceCoords.x / clusterSize.x) * (viewSpaceCoords.y / clusterSize.y));
}