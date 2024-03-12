#version 460 core
#extension GL_ARB_separate_shader_objects : enable
layout(location = 0) in vec3 fragTexCoord;
layout(location = 0) out vec4 fragColor;

layout(set = 0, binding = 0) uniform samplerCube skyboxSampler;

void main() {
    fragColor = texture(skyboxSampler, fragTexCoord);
}