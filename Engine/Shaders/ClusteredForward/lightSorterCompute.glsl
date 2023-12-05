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
    return int((round(rowCol.y * (clusterCount.x)) + (rowCol.x)));
}

vec2 indexToRowCol(int index, vec2 clusterCount) {
    float col = mod(float(index), clusterCount.x);
    float row = floor(float(index) / clusterCount.x);
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
float radius = 3.0f;
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

//vec4 screen2Eye(vec4 coord)
//{
//#if BGFX_SHADER_LANGUAGE_GLSL
//    // https://www.khronos.org/opengl/wiki/Compute_eye_space_from_window_space
//    vec3 ndc = vec3(
//        2.0 * (coord.x - u_viewRect.x) / u_viewRect.z - 1.0,
//        2.0 * (coord.y - u_viewRect.y) / u_viewRect.w - 1.0,
//        2.0 * coord.z - 1.0 // -> [-1, 1]
//    );
//#else
//    vec3 ndc = vec3(
//        2.0 * (coord.x - u_viewRect.x) / u_viewRect.z - 1.0,
//        2.0 * (u_viewRect.w - coord.y - 1 - u_viewRect.y) / u_viewRect.w - 1.0, // y is flipped
//        coord.z // -> [0, 1]
//    );
//#endif
//
//    // https://stackoverflow.com/a/16597492/862300
//    vec4 eye = mul(-projection, vec4(ndc, 1.0));
//    eye = eye / eye.w;
//
//    return eye;
//}

struct Plane
{
    vec3 Normal;
    float Distance;
};

struct Frustum
{
    Plane planes[4];
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

vec4 ClipToView(vec4 clip, mat4 inverseProjection)
{
    vec4 view = inverseProjection * clip;
    view /= view.w;
    return view;
}

vec4 ScreenToView(vec4 screen, mat4 inverseProjection)
{
    vec2 texCoord = screen.xy / screenSize;
    vec4 clip = vec4(vec2(texCoord.x, 1.f - texCoord.y) * 2.f - 1.f, screen.z, screen.w);
    return ClipToView(clip, inverseProjection);
}

bool SphereInsidePlane(Light light, Plane plane)
{
    return dot(plane.Normal, light.position) - plane.Distance < -radius;
}

bool SphereInsideFrustum(Light light, Frustum frustum, float zNear, float zFar)
{
    bool result = true;
    if(light.position.z - radius > zNear || light.position.z + radius < zFar)
    {
        result = false;
    }
    for(int i = 0; i < 4 && result; i++)
    {
        if(SphereInsidePlane(light, frustum.planes[i]))
        {
            result = false;
        }
    }
    return result;
}

const int TILE_SIZE = 32;

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main()
{
    vec2 clusterCount = ceil(screenSize / clusterSize);
    int totalClusterCount = int(ceil(clusterCount.x * clusterCount.y));
    int x = int(gl_WorkGroupID.x);
    int y = int(gl_WorkGroupID.y);
    int clusterIndex = int(x * clusterCount.y + y);

    vec4 screenSpace[4];
    screenSpace[0] = vec4(vec2(x, y) * TILE_SIZE, 0.f, 1.f);
    screenSpace[1] = vec4(vec2(x + 1, y) * TILE_SIZE, 0.f, 1.f);
    screenSpace[2] = vec4(vec2(x, y + 1) * TILE_SIZE, 0.f, 1.f);
    screenSpace[3] = vec4(vec2(x + 1, y + 1) * TILE_SIZE, 0.f, 1.f);

    const vec2 invViewDimensions = 1.0f / vec2(screenSize.x, screenSize.y);
    vec3 viewSpace[4];
    viewSpace[0] = ScreenToView(screenSpace[0], inverse(projection)).xyz;
    viewSpace[1] = ScreenToView(screenSpace[1], inverse(projection)).xyz;
    viewSpace[2] = ScreenToView(screenSpace[2], inverse(projection)).xyz;
    viewSpace[3] = ScreenToView(screenSpace[3], inverse(projection)).xyz;

    const vec3 eyePos = vec3(0.0f);
    Frustum frustum;
    frustum.planes[0] = ComputePlane(viewSpace[0], eyePos, viewSpace[2]);
    frustum.planes[1] = ComputePlane(viewSpace[3], eyePos, viewSpace[1]);
    frustum.planes[2] = ComputePlane(viewSpace[1], eyePos, viewSpace[0]);
    frustum.planes[3] = ComputePlane(viewSpace[2], eyePos, viewSpace[3]);

    frustums[clusterIndex] = frustum;

    clusters[clusterIndex].lightsCount = 0;

    for(int i = 0; i < 256; i++)
    {
         clusters[clusterIndex].lightsIndex[i] = -1;    
    }

    barrier();
    
    for(int i = 0; i < 256; i++)
    {
        Light light = lights[i];
        if(SphereInsideFrustum(light, frustum, -1, 0))
        {
            clusters[clusterIndex].lightsIndex[clusters[clusterIndex].lightsCount] = i;
            clusters[clusterIndex].lightsCount += 1;
        }
    }
    
}