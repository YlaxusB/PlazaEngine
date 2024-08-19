#version 450
layout(location = 0) in vec3 fragTexCoord;
//layout (location = 0) out vec4 gPosition;
layout (location = 0) out vec4 gNormal;
layout (location = 1) out vec4 gDiffuse;
layout (location = 2) out vec4 gOthers;

layout(binding = 7) uniform samplerCube prefilterMap;

layout(push_constant) uniform PushConstants{
    mat4 projection;
    mat4 view;
	float skyboxIntensity;
	float gamma;
	float exposure;
} pushConstants;

void main() {
    //gDiffuse = vec4(pow(texture(skyboxSampler, fragTexCoord).xyz, vec3(1.0f / pushConstants.gamma)), 1.0f);
    gDiffuse = texture(prefilterMap, fragTexCoord);
}