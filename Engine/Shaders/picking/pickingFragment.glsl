#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform float objectID;
out vec3 pixelObjectID;

void main()
{       
        pixelObjectID = vec3(objectID, objectID, objectID);
}
