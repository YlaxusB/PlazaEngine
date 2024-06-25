#version 460 core
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
    vec4 directionalLightColor;
    vec4 ambientLightColor;
    bool showCascadeLevels;
} ubo;
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in vec4 instanceMatrix[4];
layout(location = 9) in ivec4 boneIds;
layout(location = 10) in vec4 weights;
layout(location = 11) in uint vertexMaterialIndex;
//layout(location = 12) in uint materialsOffsets[64];

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out mat4 model;

const int MAX_BONES = 1000;
const int MAX_BONE_INFLUENCE = 4;
layout(std430, binding = 1) readonly buffer BoneMatrices {
	mat4 boneMatrices[];
};

//layout(std430, binding = 2) readonly buffer RenderGroups {
//    uint renderGroupOffsets[]; // This takes the instance id, returns an index that will be the starting point of the material offsets, thus converting the vertex material index (from 0 to ...) into an index that takes into account prior materials 
//	uint renderGroupMaterialsOffsets[]; // This takes the converted vertex material index and returns the real material index
//};

layout(std430, binding = 2) readonly buffer RenderGroupOffsetsBuffer {
    uint renderGroupOffsets[]; 
};

layout(std430, binding = 3) readonly buffer RenderGroupMaterialsOffsetsBuffer { 
	uint renderGroupMaterialsOffsets[];
};

//#define MESH_RENDERER_MAX_MATERIALS 64
//layout(location = 18) in uint[MESH_RENDERER_MAX_MATERIALS] meshRendererMaterials;
//layout(location = 18) in uint meshRendererMaterials;
//layout(location = 18) in uint mat;
layout(binding = 20) uniform sampler2D textures[];

layout(location = 10) out uint materialIndex;
layout(location = 11) out vec4 FragPos;
layout(location = 12) out vec4 Normal;
layout(location = 13) out vec2 TexCoords;
layout(location = 14) out vec4 TangentLightPos;
layout(location = 15) out vec4 TangentViewPos;
layout(location = 16) out vec4 TangentFragPos;
layout(location = 17) out vec4 worldPos;
layout(location = 18) out flat int affected;

out gl_PerVertex {
	vec4 gl_Position;   
};

void main() {
// TODO: FIX ANIMATION NORMALS
    model = mat4(instanceMatrix[0], instanceMatrix[1], instanceMatrix[2], instanceMatrix[3]);

    mat4 aInstanceMatrix = model;
    fragTexCoord = inTexCoord;

    mat4 finalInstanceMatrix = model;
    FragPos = vec4(model * vec4(inPosition, 1.0));
    //vs_out.Normal = transpose(inverse(mat3(aInstanceMatrix))) * aNormal;

    //if(usingNormal){
    //    mat3 normalMatrix = transpose(inverse(mat3(finalInstanceMatrix)));
    //    vec3 T = normalize(normalMatrix * aTangent);
    //    vec3 N = normalize(normalMatrix * aNormal);
    //    T = normalize(T - dot(T, N) * N);
    //    vec3 B = cross(N, T);
    //    mat3 TBN = transpose(mat3(T, B, N));    
    //    vs_out.TangentLightPos = TBN * lightPos;
    //    vs_out.TangentViewPos  = TBN * viewPos;
    //    vs_out.TangentFragPos  = TBN * vs_out.FragPos;
    //} else{
        Normal.xyz = transpose(inverse(mat3(finalInstanceMatrix))) * inNormal;
    //}

    vec4 totalPosition = vec4(0.0f);
    bool allNegative = true;

    affected = 0;
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
            if(boneIds[i] == 0)
                affected = 1;
          if(boneIds[i] < 0 || boneIds[i] > 1000) 
          {
            continue;
          }   
          else
          {
               allNegative = false;
          }
          if(boneIds[i] >= MAX_BONES) 
          {
              totalPosition = vec4(inPosition,1.0f);
              break;
          }
          vec4 localPosition = boneMatrices[boneIds[i]] * vec4(inPosition,1.0f);
          totalPosition += localPosition * weights[i];
          vec3 localNormal = mat3(boneMatrices[boneIds[i]]) * Normal.xyz;
    }
     
     if(allNegative)
         totalPosition = vec4(inPosition,1.0f);

    vec4 finalModel = finalInstanceMatrix * totalPosition;

    worldPos.xyz = vec3(finalModel);
    gl_Position = ubo.projection * ubo.view * finalModel;
    if(vertexMaterialIndex < 16536)
        materialIndex = renderGroupMaterialsOffsets[vertexMaterialIndex + renderGroupOffsets[gl_InstanceIndex]];//meshRendererMaterials[vertexMaterialIndex];//22;
    else
        materialIndex = 0;
}