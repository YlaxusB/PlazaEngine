#version 430 core
in vec3 FragPos;  // Received from the vertex shader
in vec3 pos;

uniform vec3 viewPos;
uniform mat4 viewMatrix;
struct Light {
    vec3 position;
    vec3 color;
    // Add other light properties as needed
};

layout (std430, binding = 1) buffer LightsBuffer {
    Light lights[];
};

// Add other shader inputs and uniforms as needed

out vec4 FragColor;

// Attenuate the point light intensity
float attenuate(vec3 lightDirection, float radius) {
    float cutoff = 0.5;
    float distanceToLight = length(lightDirection);
    
    // Adjust attenuation based on the light's radius
    float attenuation = distanceToLight / (radius * 0.5); // Adjust the factor as needed
    
    // Use a smoothstep function for smoother transition
    attenuation = smoothstep(0.0, 1.0, 1.0 - attenuation);
    
    return attenuation;
}

void main() {
    vec3 totalLighting = vec3(0.0);

    // Iterate through lights in the current cluster
    for (int i = 0; i < lights.length(); ++i) {
        Light light = lights[i];
        vec4 posit = viewMatrix * vec4(light.position, 1.0f);
        float radius = 15.0f;
        float distanceToLight = distance(posit.xyz, FragPos);
            float attenuation =  attenuate(posit.xyz - FragPos, radius);
            totalLighting += (light.color * attenuation);//(light.color * (a - distance(light.position, pos)));
    }

    FragColor = vec4(totalLighting, 1.0);
}