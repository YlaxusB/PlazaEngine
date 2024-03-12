#version 450
layout (location = 0) in vec3 inPos;
layout(location = 5) in vec4 instanceMatrix[4];
layout(location = 0) out vec3 fragTexCoord;

layout(push_constant) uniform PushConstants{
    mat4 projection;
    mat4 view;
} pushConstants;

void main()
{
	fragTexCoord = inPos;
	// Convert cubemap coordinates into Vulkan coordinate space
	fragTexCoord.xy *= -1.0;
	// Remove translation from view matrix
	mat4 viewMat = mat4(mat3(pushConstants.view));
	gl_Position = pushConstants.projection * viewMat * vec4(inPos.xyz, 1.0);
    //const vec3 vertices[6] = vec3[](
    //    vec3(-1.0, -1.0, 1.0),
    //    vec3(1.0, -1.0, 1.0),
    //    vec3(-1.0, 1.0, 1.0),
    //    vec3(1.0, 1.0, 1.0),
    //    vec3(-1.0, -1.0, -1.0),
    //    vec3(1.0, -1.0, -1.0)
    //);
    //vec4 worldPosition = pushConstants.view * vec4(vertices[gl_VertexIndex], 1.0);
    //fragTexCoord = worldPosition.xyz;
    //gl_Position = pushConstants.projection * worldPosition;
}