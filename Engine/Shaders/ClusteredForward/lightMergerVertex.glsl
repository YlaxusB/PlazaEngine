#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

struct Light {
    vec3 position;
    vec3 color;
    // Add other light properties as needed
};
struct Cluster {
    Light[1024] lights;
};
out Cluster[] Clusters;

layout (std430, binding = 1) buffer LightsBuffer {
    Light lights[];
};

void main()
{
    for (int i = 0; i < lights.length(); ++i)
    {
        
    }
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}