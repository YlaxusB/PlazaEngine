#version 460 
layout (location = 0) in vec2 inUV;

layout(push_constant) uniform PushConstants {
	vec4 position;
	uint cascadeIndex;
} pushConstants;


void main()
{       
	if(pushConstants.cascadeIndex > 100)
		discard;
}