#version 460 core
//#extension GL_EXT_descriptor_indexing : enable
#extension GL_EXT_nonuniform_qualifier : enable
layout(location = 0) in vec3 fragTexCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D skyboxSampler;

void main() {
    fragColor = texture( skyboxSampler, fragTexCoord.xy);//vec4(texture(skyboxSampler, vec3(fragTexCoord, 1.0f)).rgb, 1.0f);
}