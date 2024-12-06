#version 450

layout (binding = 0) uniform sampler2D normalTexture;
layout (binding = 1) uniform sampler2D sceneTexture;
layout (binding = 2) uniform sampler2D othersTexture;
layout (binding = 3) uniform sampler2D depthTexture;

layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 fragColor;

layout(push_constant) uniform PushConstants {
    vec4 screenSize;
    mat4 projection;
    mat4 view;
    mat4 lensProjection;
} pushConstants;

float GetDepthUV(vec2 uv) {
    return texture(depthTexture, uv).r;
}

vec3 ReconstructWorldPosition(vec2 uv) {
    mat4 viewProjection = pushConstants.projection * pushConstants.view;
    float ndcZ = GetDepthUV(uv) * 2.0f - 1.0f; 
    vec4 ndcPosition = vec4(gl_FragCoord.xy * (1.0f / pushConstants.screenSize.xy) * 2.0 - 1.0, (GetDepthUV(uv)), 1.0);
    ndcPosition.y *= -1;
    vec4 worldPosition = inverse(viewProjection) * ndcPosition;
    worldPosition.xyz /= worldPosition.w;
    return worldPosition.xyz;
    //vec4 clipPos = vec4(uv * 2.0f - 1.0f, depth, 1.0);
    //vec4 viewPos = inverse(pushConstants.view) * clipPos;
    //return viewPos.xyz / viewPos.w;
}

vec3 ReconstructViewPosition(vec2 uv) {
    vec4 clipPos = vec4(uv * 2.0 - 1.0, GetDepthUV(uv), 1.0);
    //clipPos.y *= -1.0f;
    vec4 viewPos = inverse(pushConstants.projection) * clipPos;
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

void main() 
{
    bool insideMiddle = inUV.x > 0.25f && inUV.x < 0.75 && inUV.y > 0.25f && inUV.y < 0.75f;
    //if(!insideMiddle) {
    //    fragColor = texture(sceneTexture, inUV);
    //    return;
    //}
        
    //vec3 normal = texture(normalTexture, inUV).xyz * 2.0 - 1.0f;
    vec4 others = texture(othersTexture, inUV);
    float metallic = others.y;
    float sceneDepth = GetDepth();
    //vec3 viewPos = ReconstructToViewPosition(inUV, sceneDepth);

    if(sceneDepth >= 1.0f || metallic < 0.4f) {
        fragColor = texture(sceneTexture, inUV);
        return;
    }
    
    float maxDistance = 18;
    float resolution  = 0.3;
    int   steps       = 16;
    float thickness   = 0.5;
    
    vec2 texSize  = pushConstants.screenSize.xy;//textureSize(positionTexture, 0).xy;
    vec2 texCoord = gl_FragCoord.xy / texSize;

    vec4 positionFrom = vec4(WorldToViewSpace(ReconstructWorldPosition(inUV)), 1.0f);
    vec3 unitPositionFrom = normalize(positionFrom.xyz);
    vec3 cameraPos = -transpose(mat3(pushConstants.view)) * vec3(pushConstants.view[3]);
    vec3 normal = normalize((texture(normalTexture, texCoord).xyz));
    vec3 pivot  = normalize(reflect(unitPositionFrom, normal));

    vec4 startView = vec4((vec3(positionFrom.xyz + (pivot *           0))), 1.0f);
    startView.y *= -1.0f;
    vec4 endView   = vec4((vec3(positionFrom.xyz + (pivot * maxDistance))), 1.0f);
    endView.y *= -1.0f;

      vec4 startFrag      = startView;
       // Project to screen space.
       startFrag      =  pushConstants.projection * startFrag;
       // Perform the perspective divide.
       startFrag.xyz /= startFrag.w;
       // Convert the screen-space XY coordinates to UV coordinates.
       startFrag.xy   = startFrag.xy * 0.5 + 0.5;
       // Convert the UV coordinates to fragment/pixel coordnates.
       startFrag.xy  *= texSize;

  vec4 endFrag      = endView;
       endFrag      =  pushConstants.projection * endFrag;
       endFrag.xyz /= endFrag.w;
       endFrag.xy   = endFrag.xy * 0.5 + 0.5;
       endFrag.xy  *= texSize;

    vec2 uv;
    vec2 frag  = startFrag.xy;
    uv.xy = frag / texSize;

    float deltaX    = endFrag.x - startFrag.x;
    float deltaY    = endFrag.y - startFrag.y;

    float useX      = abs(deltaX) >= abs(deltaY) ? 1 : 0;
    float delta     = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0, 1);

    vec2  increment = vec2(deltaX, deltaY) / max(delta, 0.001);

    float search0 = 0;
    float search1 = 0;
    
    int hit0 = 0;
    int hit1 = 0;
    
    float viewDistance = sceneDepth;///startView.y;
    float depth        = thickness;
    float i = 0;

    vec4 positionTo = positionFrom;
    for (i = 0; i < int(delta); ++i) {
        if(i > 150) {
            hit0 = 3;
            break;
        }
        frag      += increment;
        uv.xy      = frag / texSize;
        positionTo = vec4(WorldToViewSpace(ReconstructWorldPosition(inUV)), 1.0f);

        search1 =   mix((frag.y - startFrag.y) / deltaY, (frag.x - startFrag.x) / deltaX, useX);

        search1 = clamp(search1, 0.0, 1.0);

        viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
        depth        = viewDistance - positionTo.y;

        if (depth > 0 && depth < thickness) {
          hit0 = 1;
          break;
        } else {
          search0 = search1;
        }
    }

    search1 = search0 + ((search1 - search0) / 2.0);

    steps *= hit0;

    for (i = 0; i < steps; ++i) {
        frag       = mix(startFrag.xy, endFrag.xy, search1);
        uv.xy      = frag / texSize;
        positionTo = vec4(WorldToViewSpace(ReconstructWorldPosition(inUV)), 1.0f);//texture(sceneTexture, uv.xy);
        
        viewDistance = (startView.y * endView.y) / mix(endView.y, startView.y, search1);
        depth        = viewDistance - positionTo.y;
        
        if (depth > 0 && depth < thickness) {
          hit1 = 1;
          search1 = search0 + ((search1 - search0) / 2);
        } else {
          float temp = search1;
          search1 = search1 + ((search1 - search0) / 2);
          search0 = temp;
        }
    }

    if(hit0 == 1) {
        //fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
        fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    } else if (hit0 == 3) {
        fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    else {
        fragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    }

            fragColor = vec4(texture(normalTexture, texCoord).xyz, 1.0f);
            fragColor = vec4(texture(sceneTexture, inUV).xyz + texture(sceneTexture, uv).xyz * 0.3f, 1.0f);
            //fragColor = vec4(texture(sceneTexture, inUV).xyz, 1.0f);
            //fragColor = vec4(WorldToViewSpace(ReconstructWorldPosition(inUV)), 1.0f);
}


//vec3 PositionFromDepth(float depth) {
//    float z = depth * 2.0 - 1.0;
//
//    vec4 clipSpacePosition = vec4(inUV * 2.0 - 1.0, z, 1.0);
//    vec4 viewSpacePosition = (pushConstants.projection * pushConstants.view)* clipSpacePosition;
//
//    // Perspective division
//    viewSpacePosition /= viewSpacePosition.w;
//
//    return viewSpacePosition.xyz;
//}