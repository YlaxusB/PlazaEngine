#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in vec4 instanceMatrix[4];
layout(location = 0) out vec3 fragTexCoord;

layout(push_constant) uniform PushConstants{
    mat4 projection;
    mat4 view;
	uint uuid1;
	uint uuid2;
} pushConstants;


void main()
{
	mat4 model = mat4(instanceMatrix[0], instanceMatrix[1], instanceMatrix[2], instanceMatrix[3]);
	vec4 finalModel = model * vec4(inPosition.xyz, 1.0);

	mat4 viewMat = mat4(mat3(pushConstants.view));
	gl_Position = pushConstants.projection * viewMat * finalModel;
}