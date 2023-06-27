#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;
/*
void main()
{    
    FragColor = texture(texture1, TexCoords);
    //FragColor = vec4(0.3, 0.5, 0.2, 0.3);
}
*/
void main()
{
    FragColor = texture(texture1, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}   