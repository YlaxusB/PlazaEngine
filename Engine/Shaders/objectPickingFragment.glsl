#version 330 core

// Ouput data


// Values that stay constant for the whole mesh.
uniform float objectID;
out vec3 pixelObjectID;

void main(){
    pixelObjectID = vec3(objectID, objectID, objectID);
}