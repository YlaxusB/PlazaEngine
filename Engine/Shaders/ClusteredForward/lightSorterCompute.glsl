#version 430 core
struct Light{
    vec4 color;
    vec3 position;
    float radius;
    float cutoff;
};

layout(std430, binding = 0) buffer LightsArray {
    Light lights[];
};

struct Cluster{
    int[256] lightsIndex;
    int lightsCount;
    vec3 minBounds;
    vec3 maxBounds;
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
    return int((ceil(rowCol.y * (clusterCount.x)) + (rowCol.x)));
}

vec2 indexToRowCol(int index, vec2 clusterCount) {
    float col = mod(float(index), clusterCount.x);
    float row = ceil(float(index) / clusterCount.x);
    return vec2(row, col);
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

bool GetProjectedBounds(vec3 center, float radius, inout vec3 boxMin, inout vec3 boxMax)
{

    /// frustum culling helper

    float d2 = dot(center,center);

    float a = sqrt(d2 - radius * radius);

    /// view-aligned "right" vector (right angle to the view plane from the center of the sphere. Since  "up" is always (0,n,0), replaced cross product with vec3(-c.z, 0, c.x)
    vec3 right = (radius / a) * vec3(-center.z, 0, center.x);
    vec3 up = vec3(0,radius,0);

    vec4 projectedRight  = projection * vec4(right,0);
    vec4 projectedUp     = projection * vec4(up,0);

    vec4 projectedCenter = projection * vec4(center,1);

    vec4 north  = projectedCenter + projectedUp;
    vec4 east   = projectedCenter + projectedRight;
    vec4 south  = projectedCenter - projectedUp;
    vec4 west   = projectedCenter - projectedRight;

    north /= north.w ;
    east  /= east.w  ;
    west  /= west.w  ;
    south /= south.w ;

    boxMin = min(min(min(east,west),north),south).xyz;
    boxMax = max(max(max(east,west),north),south).xyz;


    return true;
}
float radius = 25.1f;
bool pointLightIntersectsCluster(Light light, Cluster cluster)
{
    // NOTE: expects light.position to be in view space like the cluster bounds
    // global light list has world space coordinates, but we transform the
    // coordinates in the shared array of lights after copying

    // get closest point to sphere center
    vec3 closest = max(cluster.minBounds, min(light.position, cluster.maxBounds));
    // check if point is inside the sphere
    vec3 dist = closest - light.position;
    return dot(dist, dist) <= (radius * radius);    
}

struct Plane
{
    vec3 Normal;
    float Distance;
};

struct Frustum
{
    vec4 planes[4];
};

layout (std430, binding = 7) buffer FrustumsBuffer {
    Frustum frustums[];
};

Plane ComputePlane(vec3 p0, vec3 p1, vec3 p2)
{
    Plane plane;
    const vec3 v0 = p1 - p0;
    const vec3 v2 = p2 - p0;

    plane.Normal = normalize(cross(v0, v2));
    plane.Distance = dot(plane.Normal, p0);
    return plane;
}

vec4 ClipToView(vec4 clip)
{
    vec4 view = inverse(projection) * clip;
    view /= view.w;
    return view;
}

vec4 ScreenToView(vec4 screen)
{
    vec2 texCoord = screen.xy * (1 / screenSize);
    vec4 clip = vec4(vec2(texCoord.x, 1.f - texCoord.y) * 2.f - 1.f, screen.z, screen.w);
    return ClipToView(clip);
}

bool SphereInsidePlane(Light light, vec4 plane)
{
    return dot(plane.xyz, light.position) - plane.w < -radius;
}

bool SphereInsideFrustum(Light light, Frustum frustum, float zNear, float zFar)
{
    bool result = true;
 
    // First check depth
    // Note: Here, the view vector points in the -Z axis so the 
    // far depth value will be approaching -infinity.
    if ( light.position.z - radius > zNear || light.position.z + radius < zFar )
    {
        result = false;
    }

    // Then check frustum planes
    for ( int i = 0; i < 4 && result; i++ )
    {
        if ( SphereInsidePlane( light, frustum.planes[i] ) )
        {
            result = false;
        }
    }
 
    return result;
   //bool insidePlane = SphereInsidePlane(light, frustum.planes[0]) || SphereInsidePlane(light, frustum.planes[1]) || SphereInsidePlane(light, frustum.planes[2]) || SphereInsidePlane(light, frustum.planes[3]);
   // //return !((light.position.z - radius < zNear || light.position.z + radius < zFar) || insidePlane);
   // return !insidePlane;
}

layout (std430, binding = 8) buffer DepthTileBuffer {
    vec2 tileDepthStats[];
};
//layout (location = 30) uniform sampler2D depthMap;
uniform sampler2D depthMap;

const int tileSizeX = 32;
const int tileSizeY = 32;
vec2 calculateTileDepthStats(int tileX, int tileY) {
    vec2 clusterCount = ceil(screenSize / clusterSize);
    ivec2 tileCoords = ivec2(tileX * tileSizeX, tileY * tileSizeY);
    
    float minDepth = 0x7f7fffff;//= texture(depthMap, vec2(tileCoords) / textureSize(depthMap, 0)).r;
    float maxDepth = 0.0f;//= minDepth;

    for (int i = 0; i < tileSizeX; ++i) {
        for (int j = 0; j < tileSizeY; ++j) {
            ivec2 texCoords = tileCoords + ivec2(i, j);
            float depth = texture(depthMap, vec2(texCoords) / textureSize(depthMap, 0)).r;
            float depthVS = -ClipToView(vec4(0.0f, 0.0f, depth, 1.0f)).z;

            if(depth != 0)
            {
            
            minDepth = min(minDepth, depthVS);
            maxDepth = max(maxDepth, depthVS);
            }

        }
    }
    return vec2(minDepth, maxDepth);
}


shared uint minDepthInt;
shared uint maxDepthInt;
shared uint visibleLightCount;
shared vec4 frustumPlanes[6];

shared int visibleLightIndices[1024];

shared mat4 viewProjection;
const int TILE_SIZE = 32;

vec4 calculateFrustumPlane(vec4 col1, vec4 col2, vec4 col3, vec4 col4)
{
    vec3 normal = normalize(cross(col1.xyz, col2.xyz));
    float distance = -dot(normal, col3.xyz) / dot(normal, normal);
    return vec4(normal, distance);
}

Frustum calculateFrustum(mat4 viewProj)
{
    Frustum frustum;
    
    // Extract frustum planes in view space
    frustum.planes[0] = calculateFrustumPlane(viewProj[3] + viewProj[0], viewProj[3] - viewProj[0], viewProj[3] + viewProj[1], viewProj[3] - viewProj[1]); // Left
    frustum.planes[1] = calculateFrustumPlane(viewProj[3] + viewProj[0], viewProj[3] - viewProj[0], viewProj[3] + viewProj[2], viewProj[3] - viewProj[2]); // Bottom
    frustum.planes[2] = calculateFrustumPlane(viewProj[3] - viewProj[0], viewProj[3] - viewProj[0], viewProj[3] - viewProj[1], viewProj[3] - viewProj[1]); // Right
    frustum.planes[3] = calculateFrustumPlane(viewProj[3] - viewProj[0], viewProj[3] - viewProj[0], viewProj[3] - viewProj[2], viewProj[3] - viewProj[2]); // Top
    
    return frustum;
}

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main()
{

vec2 clusterCount = ceil(screenSize / clusterSize);
 vec4 frustumPlanes[6];
	ivec2 location = ivec2(gl_GlobalInvocationID.xy);
	ivec2 itemID = ivec2(gl_LocalInvocationID.xy);
	ivec2 tileID = ivec2(gl_WorkGroupID.xy);
	ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
	uint index = tileID.y * tileNumber.x + tileID.x;

	// Initialize shared global values for depth and light count
	if (gl_LocalInvocationIndex == 0) {
		minDepthInt = 0xFFFFFFFF;
		maxDepthInt = 0;
		visibleLightCount = 0;
		viewProjection = projection * view;
	}

	barrier();

	// Step 1: Calculate the minimum and maximum depth values (from the depth buffer) for this group's tile
	float maxDepth, minDepth;
	vec2 text = vec2(location) / ivec2(screenSize.x, screenSize.y);
	float depth = texture(depthMap, text).r;
	// Linearize the depth value from depth buffer (must do this because we created it using projection)
	//depth = projection * depth;//(0.5 * projection[3][2]) / (depth + 0.5 * projection[2][2] );

	// Convert depth to uint so we can do atomic min and max comparisons between the threads
	uint depthInt = floatBitsToUint(depth);
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);

    barrier();

    if (gl_LocalInvocationIndex == 0) {
        // Convert the min and max across the entire tile back to float
        minDepth = uintBitsToFloat(minDepthInt);
        maxDepth = uintBitsToFloat(maxDepthInt);
        
		// Steps based on tile sale
		vec2 negativeStep = (2.0 * vec2(tileID)) / vec2(tileNumber);
		vec2 positiveStep = (2.0 * vec2(tileID + ivec2(1, 1))) / vec2(tileNumber);

		// Set up starting values for planes using steps and min and max z values
		frustumPlanes[0] = vec4(1.0, 0.0, 0.0, 1.0 - negativeStep.x); // Left
		frustumPlanes[1] = vec4(-1.0, 0.0, 0.0, -1.0 + positiveStep.x); // Right
		frustumPlanes[2] = vec4(0.0, 1.0, 0.0, 1.0 - negativeStep.y); // Bottom
		frustumPlanes[3] = vec4(0.0, -1.0, 0.0, -1.0 + positiveStep.y); // Top
		frustumPlanes[4] = vec4(0.0, 0.0, -1.0, -minDepth); // Near
		frustumPlanes[5] = vec4(0.0, 0.0, 1.0, maxDepth); // Far

		// Transform the first four planes
		for (uint i = 0; i < 4; i++) {
			frustumPlanes[i] *= viewProjection;
			frustumPlanes[i] /= length(frustumPlanes[i].xyz);
		}

		// Transform the depth planes
		frustumPlanes[4] *= view;
		frustumPlanes[4] /= length(frustumPlanes[4].xyz);
		frustumPlanes[5] *= view;
		frustumPlanes[5] /= length(frustumPlanes[5].xyz);
	}
    barrier();
        
    int totalClusterCount = int(ceil(clusterCount.x * clusterCount.y));
    int x = int(gl_WorkGroupID.x);
    int y = int(gl_WorkGroupID.y);
    int clusterIndex = int(y * clusterCount.x + x);

    uint threadCount = TILE_SIZE * TILE_SIZE;
    int lightCount = 256;

    tileDepthStats[index] = vec2(minDepth, maxDepth);
    if (gl_LocalInvocationIndex == 0) {
            clusters[clusterIndex].lightsCount = 0;
    for(int i = 0; i < lightCount; i++)
    {   
        clusters[index].lightsIndex[i] = -1;    
    }
    barrier();
	uint passCount = (lightCount + threadCount - 1) / threadCount;
    if (gl_LocalInvocationIndex == 0){
	for (uint i = 0; i < lightCount; i++) {
		// Get the lightIndex to test for this thread / pass. If the index is >= light count, then this thread can stop testing lights
		uint lightIndex = i; // threadCount + gl_LocalInvocationIndex;
		if (lightIndex >= lightCount) {
			break;
		}

		vec4 position = vec4(lights[lightIndex].position, 1.0f);

		// We check if the light exists in our frustum
		float distance = 0.0;
		for (uint j = 0; j < 6; j++) {
			distance = dot(position, frustumPlanes[j]) + radius;

			// If one of the tests fails, then there is no intersection
			if (distance <= 0.0) {
				break;
			}
		}

		// If greater than zero, then it is a visible light
		if (distance > 0.0) {
			// Add index to the shared array of visible indices
                        //int lightIndex = int(clusters[index].lightsCount);

			uint offset = visibleLightCount; 
			visibleLightIndices[offset] = int(lightIndex);
            offset = atomicAdd(visibleLightCount, 1);
            //atomicAdd(clusters[clusterIndex].lightsCount, 1);
            //int index = int(clusters[clusterIndex].lightsCount) - 1;
            //clusters[clusterIndex].lightsIndex[index] = i;
		}
	}
    }
	barrier();


		uint offset = index * lightCount; // Determine bosition in global buffer
		for (uint i = 0; i < visibleLightCount; i++) {
            int lightIndex = int(clusters[index].lightsCount);
            clusters[index].lightsIndex[lightIndex] = int(visibleLightIndices[i]);
            clusters[index].lightsCount += 1;
			//visibleLightIndicesBuffer.data[offset + i].index = visibleLightIndices[i];
		}
	}
    ////barrier();
   //// if (gl_LocalInvocationIndex == 0){
    //for(int i = 0; i < clusterIndex; i++)
    //{
    //  //clusters[clusterIndex].lightsCount += 1;
    //  //clusters[clusterIndex].lightsIndex[ clusters[clusterIndex].lightsCount - 1] = i;
    //  //visibleLightIndicesBuffer.data[offset + i].index = visibleLightIndices[i];
    //}
    
    //for(int i = 0; i < lightCount; i++)
    //{
    //    Light light = lights[i];
    //    vec4 position = vec4(lights[i].position, 1.0f);
    //    float radius = 15.0f;
    //
    //    float distance = 0.0;
    //    for (uint j = 0; j < 6; j++) {
	//		distance = dot(position, frustumPlanes[j]) + radius;
    //
	//		// If one of the tests fails, then there is no intersection
	//		if (distance <= 0.0) {
	//			break;
	//		}
	//	}
    //    //if(SphereInsideFrustum(light, frustums[clusterIndex], tileDepthStats[clusterIndex].x, tileDepthStats[clusterIndex].y))
    //    if(distance > 0.0)
    //    {
    //        atomicAdd(clusters[clusterIndex].lightsCount, 1);
    //        int index = int(clusters[clusterIndex].lightsCount) - 1;
    //        clusters[clusterIndex].lightsIndex[index] = i;
    //    }
    //}
	//uint passCount = (lightCount + threadCount - 1) / threadCount;
    
    /*
	for (uint i = 0; i < passCount; i++) {
		// Get the lightIndex to test for this thread / pass. If the index is >= light count, then this thread can stop testing lights
		uint lightIndex = i * threadCount + gl_LocalInvocationIndex;
		if (lightIndex >= lightCount) {
			break;
		}

		vec4 position = vec4(lights[lightIndex].position, 1.0f);
		float radius = 1.0f;//lightBuffer.data[lightIndex].paddingAndRadius.w;

		// We check if the light exists in our frustum
		float distance = 0.0;
		for (uint j = 0; j < 6; j++) {
			distance = dot(position, frustumPlanes[j]) + radius;

			// If one of the tests fails, then there is no intersection
			if (distance <= 0.0) {
				break;
			}
		}

		// If greater than zero, then it is a visible light
		if (distance > 0.0) {
			// Add index to the shared array of visible indices
            atomicAdd(clusters[clusterIndex].lightsCount, 1);
            int index = int(clusters[clusterIndex].lightsCount) - 1;
            clusters[clusterIndex].lightsIndex[lightIndex] = int(lightIndex);

		}
	}
    
	barrier();

    /*
     //------------------------ 
    vec2 clusterCount = ceil(screenSize / clusterSize);
    // Calculate the tile coordinates based on the global thread ID
    int tileX = int(gl_GlobalInvocationID.x / tileSizeX);
    int tileY = int(gl_GlobalInvocationID.y / tileSizeY);

    // Calculate the minimum and maximum depth values for the tile
    calculateTileDepthStats(tileX, tileY);
    
    // Synchronize threads if needed
    barrier();

        if (gl_GlobalInvocationID.x == 0 && gl_GlobalInvocationID.y == 0) {
        float globalMinDepth = tileDepthStats[0].x;
        float globalMaxDepth = tileDepthStats[0].y;

        for (int i = 1; i < (clusterCount.x / tileSizeX) * (clusterCount.y / tileSizeY); ++i) {
            globalMinDepth = min(globalMinDepth, tileDepthStats[i].x);
            globalMaxDepth = max(globalMaxDepth, tileDepthStats[i].y);
        }

        // Now globalMinDepth and globalMaxDepth contain the overall minimum and maximum depths
    }



    
    int totalClusterCount = int(ceil(clusterCount.x * clusterCount.y));
    int x = int(gl_WorkGroupID.x);
    int y = int(gl_WorkGroupID.y);
    int clusterIndex = int(x * clusterCount.y + y);

    vec2 minMax = calculateTileDepthStats(x, y);
    float minDepthInt = (minMax.x);
    float maxDepthInt = (minMax.y);
    barrier();

    int x1 = int(x * 1);
    int y1 = int(y * 1);
    vec4 screenSpace[4];
    screenSpace[0] = vec4(vec2(x1, y1) * TILE_SIZE, -1.0f, 1.0f);
    screenSpace[1] = vec4(vec2(x1 + 1, y1) * TILE_SIZE, -1.0f, 1.0f);
    screenSpace[2] = vec4(vec2(x1, y1 + 1) * TILE_SIZE, -1.0f, 1.0f);
    screenSpace[3] = vec4(vec2(x1 + 1, y1 + 1) * TILE_SIZE, -1.0f, 1.0f);

    const vec2 invViewDimensions = 1.0f / screenSize;
    vec3 viewSpace[4];
    // Now convert the screen space points to view space
    for ( int i = 0; i < 4; i++ )
    {
        viewSpace[i] = ScreenToView( screenSpace[i] ).xyz;
    }

    const vec3 eyePos = vec3(0.0f);
    Frustum frustum = calculateFrustum(projection * view);
    //frustum.planes[0] = ComputePlane(viewSpace[0], eyePos, viewSpace[2]);
    //frustum.planes[1] = ComputePlane(viewSpace[3], eyePos, viewSpace[1]);
    //frustum.planes[2] = ComputePlane(viewSpace[1], eyePos, viewSpace[0]);
    //frustum.planes[3] = ComputePlane(viewSpace[2], eyePos, viewSpace[3]);

    frustums[clusterIndex] = frustum;

//    calculateFrustumPlanes(projection * view, clusterIndex);

    clusters[clusterIndex].lightsCount = 0;

    for(int i = 0; i < 1024; i++)
    {
         clusters[clusterIndex].lightsIndex[i] = -1;    
    }
    barrier();
   
   tileDepthStats[clusterIndex] = minMax;
    
    for(int i = 0; i < 1024; i++)
    {
        Light light = lights[i];
        if(SphereInsideFrustum(light, frustums[clusterIndex], minDepthInt, maxDepthInt))
        {
            atomicAdd(clusters[clusterIndex].lightsCount, 1);
            int index = int(clusters[clusterIndex].lightsCount) - 1;
            clusters[clusterIndex].lightsIndex[index] = i;
        }
    }
    */
}