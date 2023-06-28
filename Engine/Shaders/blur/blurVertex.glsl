#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos * 0.5 + 0.5;
    //TexCoords = aTexCoords;  // Modify this line to assign only the x and y components
    //gl_Position = pos.xyww;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}