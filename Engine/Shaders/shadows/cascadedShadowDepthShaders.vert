#version 450
layout (location = 0) in vec3 aPos;
layout(location = 5) in vec4 instanceMatrix[4];

void main()
{
    mat4 model = mat4(instanceMatrix[0], instanceMatrix[1], instanceMatrix[2], instanceMatrix[3]);
    gl_Position = model * vec4(aPos, 1.0);
}