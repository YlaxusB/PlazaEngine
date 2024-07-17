#version 450

layout(location = 0) in vec3 WorldPos;
layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183); // 1/(2 * PI) and 1/PI

void main() {
    vec3 dir = normalize(WorldPos);
    vec2 uv = vec2(atan(dir.z, dir.x) * invAtan.x + 0.5, asin(dir.y) * invAtan.y + 0.5);
    FragColor = texture(equirectangularMap, uv);
}