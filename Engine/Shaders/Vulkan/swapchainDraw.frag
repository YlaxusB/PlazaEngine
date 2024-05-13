#version 450

layout (binding = 1) uniform sampler2D samplerTexture;
layout (location = 0) in vec2 inUV;
layout (location = 0) out vec4 outFragcolor;

void main() 
{
	outFragcolor = vec4(texture(samplerTexture, inUV));
}