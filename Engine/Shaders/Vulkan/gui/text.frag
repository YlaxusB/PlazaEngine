#version 450 core

layout (location = 0) in vec2 inUV;

layout (binding = 0) uniform sampler2D samplerFont;

layout (location = 0) out vec4 outFragColor;

void main(void)
{
	float color = texture(samplerFont, vec2(inUV.x, abs(inUV.y))).r;
	outFragColor = vec4(color);
}
