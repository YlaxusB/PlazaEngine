#version 450 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout (location = 0) out vec2 outUV;

layout(push_constant) uniform PushConstants{
    mat4 matrix;
} pushConstants;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main(void)
{
	gl_Position = pushConstants.matrix * vec4(inPosition.xz * vec2(0.5f, 0.2f) + vec2(910, 1) / vec2(1820, 720), 0.0f, 1.0f);
	outUV = inTexCoord;
}
