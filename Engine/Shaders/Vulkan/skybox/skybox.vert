#version 460 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) out vec3 fragTexCoord;

layout(push_constant) uniform PushConstants{
    mat4 projection;
    mat4 view;
} pushConstants;

void main()
{
    const vec3 vertices[6] = vec3[](
        vec3(-1.0, -1.0, 1.0),
        vec3(1.0, -1.0, 1.0),
        vec3(-1.0, 1.0, 1.0),
        vec3(1.0, 1.0, 1.0),
        vec3(-1.0, -1.0, -1.0),
        vec3(1.0, -1.0, -1.0)
    );
    vec4 worldPosition = pushConstants.view * vec4(vertices[gl_VertexIndex], 1.0);
    fragTexCoord = worldPosition.xyz;
    gl_Position = pushConstants.projection * worldPosition;
}