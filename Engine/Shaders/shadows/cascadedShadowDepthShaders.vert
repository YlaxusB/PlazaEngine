#version 460
#extension GL_EXT_multiview : enable

layout (location = 0) in vec3 aPos;
layout(location = 5) in vec4 instanceMatrix[4];
layout(location = 0) out vec2 outUV;

layout (binding = 0) uniform UBO 
{
	mat4 lightSpaceMatrices[32];
} ubo;

layout(push_constant) uniform PushConstants {
	vec4 position;
	uint cascadeIndex;
} pushConstants;

out gl_PerVertex {
	vec4 gl_Position;   
};

const mat4 biasMat = mat4( 
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 
);

void main()
{
    mat4 model = mat4(instanceMatrix[0], instanceMatrix[1], instanceMatrix[2], instanceMatrix[3]);
	vec4 pos = model * vec4(aPos, 1.0f);
    gl_Position = ubo.lightSpaceMatrices[gl_ViewIndex] * pos;
}