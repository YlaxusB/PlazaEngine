#version 450

layout (binding = 0) uniform sampler2D normalTexture;
layout (binding = 1) uniform sampler2D sceneTexture;
layout (binding = 2) uniform sampler2D othersTexture;
layout (binding = 3) uniform sampler2D depthTexture;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 fragColor;

layout(push_constant) uniform PushConstants {
    vec4 screenSize;
    vec4 cameraPos;
    mat4 projection;
    mat4 view;
    mat4 lensProjection;
} pushConstants;

float GetDepthUV(vec2 uv) {
//uv.y = 1.0f - uv.y;
    return texture(depthTexture, uv).r;
}

vec3 ReconstructWorldPosition(vec2 uv) {
    mat4 viewProjection = pushConstants.projection * pushConstants.view;
    float ndcZ = GetDepthUV(uv) * 2.0f - 1.0f; 
    vec4 ndcPosition = vec4(uv * 2.0 - 1.0, (GetDepthUV(uv)), 1.0);
    ndcPosition.y *= -1.0f;
    vec4 worldPosition = inverse(viewProjection) * ndcPosition;
    worldPosition.xyz /= worldPosition.w;
    return worldPosition.xyz;
    //vec4 clipPos = vec4(uv * 2.0f - 1.0f, depth, 1.0);
    //vec4 viewPos = inverse(pushConstants.view) * clipPos;
    //return viewPos.xyz / viewPos.w;
}

vec3 ReconstructViewPosition(vec2 uv) {
    mat4 viewProjection = pushConstants.projection * pushConstants.view;
    vec4 clipPos = vec4(uv * 2.0 - 1.0, GetDepthUV(uv), 1.0);
    //clipPos.y *= -1.0f;
    vec4 viewPos = inverse(viewProjection) * clipPos;
    return viewPos.xyz / viewPos.w;
}


float GetDepth() {
    return texture(depthTexture, inUV).r;
}

//vec3 ReconstructViewPosition(vec2 uv, float depth) {
//    vec4 clipPos = vec4(uv * 2.0f - 1.0f, depth, 1.0);
//     clipPos.y *= -1;
//    vec4 viewPos = inverse(pushConstants.view) * clipPos;
//    return viewPos.xyz / viewPos.w;
//}

vec3 WorldToViewSpace(vec3 worldPosition) {
    vec4 viewSpacePosition = pushConstants.view * vec4(worldPosition, 1.0);
    return viewSpacePosition.xyz; // Convert back to vec3
}

vec4 WorldToClipSpace(vec3 worldPosition) {
    vec4 clipSpacePosition = pushConstants.projection * pushConstants.view * vec4(worldPosition, 1.0);
    return clipSpacePosition; // Keep as vec4 for clip space
}

vec3 ClipToNDC(vec4 clipSpacePosition) {
    return clipSpacePosition.xyz / clipSpacePosition.w;
}

vec3 WorldToNDC(vec3 worldPosition) {
    // World to clip space
    vec4 clipSpacePosition = pushConstants.projection * pushConstants.view * vec4(worldPosition, 1.0);
    // Clip to NDC
    return clipSpacePosition.xyz / clipSpacePosition.w; // Perspective divide
}

vec3 reconstructViewPos(float depth, vec2 uv, mat4 invProj, vec2 screenSize) {
    vec2 ndc = uv * 2.0 - 1.0;
    vec4 clipSpacePos = vec4(ndc, depth, 1.0);
    clipSpacePos.y *= -1.0f;
    vec4 viewSpacePos = invProj * clipSpacePos;
    viewSpacePos /= max(viewSpacePos.w, 0.0001); // Prevent divide-by-zero
    return viewSpacePos.xyz;
}

void main() {
    // Step 1: Read inputs and set up basic data
    vec2 texSize = pushConstants.screenSize.xy;
    vec2 texCoord = clamp(inUV, vec2(0.0), vec2(1.0)); // Clamp UV to avoid out-of-bounds access
    float sceneDepth = GetDepthUV(texCoord);

    // Return early if depth is at maximum (background or invalid data)
    if (sceneDepth >= 1.0f) {
        fragColor = texture(sceneTexture, texCoord);
        return;
    }

    // Step 2: Reconstruct view-space position from depth
    vec3 viewPos = reconstructViewPos(sceneDepth, texCoord, inverse(pushConstants.projection), texSize);
    // Read and normalize the normal map
    vec3 normal = normalize(texture(normalTexture, texCoord).xyz);
    // Step 3: Compute the reflection vector
    vec3 unitViewPos = normalize(viewPos);
// Step 2: Reconstruct world-space position and normal
vec4 ndcPos = vec4(viewPos, 1.0f);
//ndcPos.y *= -1.0f; //+ 1.0f;
vec3 worldPos = (inverse(pushConstants.view) * ndcPos).xyz; // Transform viewPos to world space
mat3 normalMatrix = transpose((mat3(pushConstants.view)));
vec3 worldNormal = normalize(normalMatrix * normal);

// Step 3: Compute the reflection vector in world space
vec3 unitWorldViewPos = normalize(worldPos - pushConstants.cameraPos.xyz); // Direction from camera to the point in world space
vec3 worldPivot = normalize(reflect(unitWorldViewPos, worldNormal)); // Reflection in world space

// Step 4: Transform the reflection vector back to view space
mat3 piv = mat3(pushConstants.view);
vec3 pivot = ((piv) * worldPivot); // Transform reflection vector back to view space


//fragColor = vec4(vec3(reconstructViewPos(GetDepthUV(inUV.xy), inUV.xy, inverse(pushConstants.projection), texSize)).z), 1.0f);
//return;


    // Step 4: Define ray marching parameters
    float maxDistance = 16.0;
    float resolution = 0.3;
    int steps = 4;
    float thickness = 0.5;

    // Define start and end positions in view space
    vec4 startView = vec4(viewPos, 1.0);
    vec4 endView = vec4(viewPos + pivot * maxDistance, 1.0);

    // Project start and end view-space positions to screen space
    vec4 startFrag = (pushConstants.projection * startView);
    startFrag.xyz /= max(startFrag.w, 0.0001); // Perspective divide
    startFrag.xy = (startFrag.xy * 0.5 + 0.5) * texSize;

    vec4 endFrag = (pushConstants.projection * endView);
    endFrag.xyz /= max(endFrag.w, 0.0001); // Perspective divide
    endFrag.xy = (endFrag.xy * 0.5 + 0.5) * texSize;

    // Step 5: Initialize ray marching variables
    vec2 frag = startFrag.xy;
    vec2 increment = (endFrag.xy - startFrag.xy) / float(steps);

    float viewDistance = reconstructViewPos(GetDepthUV(inUV), inUV, inverse(pushConstants.projection), texSize).z;//sceneDepth;
    float depth = 0.0;
    bool hit = false;

    vec3 viewPosTo = viewPos;
    // Step 6: Perform ray marching
    for (int i = 0; i < steps; ++i) {
        // Advance the ray along the reflection vector
        frag += increment;
        vec2 uv = frag / texSize;

        // Clamp UV to prevent sampling out of bounds
        uv = clamp(uv, vec2(0.0), vec2(1.0));

        // Reconstruct view-space position at this sample point
        uv.y = 1.0f - uv.y;
        viewPosTo = reconstructViewPos(GetDepthUV(uv), uv, inverse(pushConstants.projection), texSize);
        // Compute the depth difference
        depth = viewDistance - viewPosTo.z;

        if (depth > 0.0 && depth < thickness) {
            hit = true;
            break;
        }
    }

    // Step 7: Compute visibility based on ray hit
    float visibility = 1.0;
   if (hit) {
       visibility = 1.0
          * (1.0 - max(dot(-unitViewPos, pivot), 0.0)) // Angle effect
           * (1.0 - clamp(depth / thickness, 0.0, 1.0)) // Thickness effect
           * (1.0 - clamp(length(viewPosTo - viewPos) / maxDistance, 0.0, 1.0)); // Distance effect
   }
    //visibility = clamp(visibility, 0.0, 1.0);
    //frag.y = 1.0f - frag.y;
    // Step 8: Combine base color with reflection
    vec3 baseColor = texture(sceneTexture, texCoord).rgb;
    frag = frag / texSize;
    frag.y = 1.0f - frag.y;
    vec3 reflectionColor = texture(sceneTexture, frag).rgb;

    fragColor = vec4(baseColor + reflectionColor * visibility * 0.3, 1.0);
   // fragColor = vec4(visibility, visibility, visibility, 1.0f);
}