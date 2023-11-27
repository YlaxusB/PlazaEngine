#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform sampler2D lightTexture;

void main()
{             
    FragColor = vec4(pow(vec3(texture(sceneTexture, TexCoords)), vec3(2.2f)) * vec3(1 + texture(lightTexture, TexCoords)), 1.0);
}