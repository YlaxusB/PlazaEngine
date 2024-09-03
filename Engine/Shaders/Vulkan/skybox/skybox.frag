#version 450
layout(location = 0) in vec3 fragTexCoord;
//layout (location = 0) out vec4 gPosition;
layout (location = 0) out vec4 gNormal;
layout (location = 1) out vec4 gDiffuse;
layout (location = 2) out vec4 gOthers;

layout(binding = 7) uniform samplerCube prefilterMap;
layout(binding = 10) uniform sampler2D equirectangularMap;

layout(push_constant) uniform PushConstants{
    mat4 projection;
    mat4 view;
	float skyboxIntensity;
	float gamma;
	float exposure;
    float useless;
} pushConstants;

vec3 Uncharted2Tonemap(vec3 color)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	return ((color*(A*color+C*B)+D*E)/(color*(A*color+B)+D*F))-E/F;
}

void main() {
    //gDiffuse = vec4(pow(texture(prefilterMap, fragTexCoord).xyz, vec3(1.0f / pushConstants.gamma)), 1.0f);
	vec3 color =  texture(equirectangularMap, fragTexCoord.xy).xyz;
	//if(fragTexCoord.y > 0.5f){
	//	color = mix(vec3(0.4f, 0.4f, 0.4f), vec3(0.6f, 0.6f, 0.6f), smoothstep(1.0f, 0.0f, fragTexCoord.y) * 2.0 - 1.0);//vec3(0.0f, 0.0f, 0.0f);
	//}
	//color = Uncharted2Tonemap(color * pushConstants.exposure);
	//color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	//// Gamma correction
	//color = pow(color, vec3(1.0f / pushConstants.gamma));
    gDiffuse = vec4(color, 1.0f);
}