#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 10) uniform sampler2D textures[];

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in mat4 model;

layout(push_constant) uniform PushConstants {
    vec3 color;
    float intensity;
    int diffuseIndex;
} pushConstants;

layout(location = 0) out vec4 outColor;

void main() {
    if(pushConstants.diffuseIndex > -1)
    {
        outColor = texture(textures[pushConstants.diffuseIndex], fragTexCoord);;
    }
    else
    {
        outColor = vec4(pushConstants.color, 1.0f);
    }
}