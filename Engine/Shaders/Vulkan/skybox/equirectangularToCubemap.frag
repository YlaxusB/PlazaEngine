#version 450

layout(location = 0) in vec3 WorldPos;
layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform sampler2D equirectangularMap;
//layout(binding = 2) uniform samplerCube environmentMap;

const float PI = 3.14159265359;

layout(push_constant) uniform PushConstants{
    mat4 mvp;
    bool first;
    float deltaPhi;
	float deltaTheta;
    float roughness;
    uint numSamples;
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

const vec2 invAtan = vec2(0.1591, 0.3183); // 1/(2 * PI) and 1/PI

void main() {
    vec3 dir = normalize(WorldPos);
    vec2 uv = vec2(atan(dir.z, dir.x) * invAtan.x + 0.5, asin(dir.y) * invAtan.y + 0.5);

	vec3 color = texture(equirectangularMap, uv).rgb;

	// Tone mapping
	color = Uncharted2Tonemap(color * 4.5);
	color = color * (1.0f / Uncharted2Tonemap(vec3(11.2f)));	
	// Gamma correction
	color = pow(color, vec3(1.0f / 2.2));
	
	FragColor = vec4(color, 1.0);
}