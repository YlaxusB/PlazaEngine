#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    

    vec3 texDiffuseCol = texture2D(texture_diffuse1, TexCoords).rgb;
    if(length(texDiffuseCol) == 0.0)
    {
         FragColor = vec4(0.8, 0.3, 0.3, 1);
    } 
    else
    {
        FragColor = texture(texture_diffuse1, TexCoords);
    }
}