#version 330 core

out vec4 FragColor;

uniform sampler2D texture_diffuse1;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{    
vec3 color;
vec3 texDiffuseCol = texture2D(texture_diffuse1, fs_in.TexCoords).rgb;
    if(length(texDiffuseCol) == 0.0)
    {
         //FragColor = vec4(0.8, 0.3, 0.3, 1);
         color = vec3(0.8, 0.3, 0.3);
    } 
    else
    {
        color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    }

    // ambient
    vec3 ambient = 0.20 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;

    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0);

/*   vec3 texDiffuseCol = texture2D(texture_diffuse1, TexCoords).rgb;
    if(length(texDiffuseCol) == 0.0)
    {
         FragColor = vec4(0.8, 0.3, 0.3, 1);
    } 
    else
    {
        FragColor = texture(texture_diffuse1, TexCoords);
    }*/
   
}
