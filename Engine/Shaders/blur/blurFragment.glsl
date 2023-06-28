#version 330 core

uniform sampler2D frameBufferTexture;
uniform usampler2D stencilTexture;
uniform int numSteps = 12;
uniform float radius = 3.0;

const float TAU = 6.28318530;

in vec2 TexCoords;
out vec4 FragColor;

// Modification of: https://www.shadertoy.com/view/sltcRf
void main()
{
    vec4 color = vec4(texture(frameBufferTexture, TexCoords).rgb, 1);
    uint stencil = texture(stencilTexture, TexCoords).r;
    
    vec2 aspect = 1.0 / vec2(textureSize(stencilTexture, 0));
    float outlinemask = 0.0;
    for (float i = 0.0; i < TAU; i += TAU / numSteps)
    {
        vec2 offset = vec2(sin(i), cos(i)) * aspect * radius;
        float col = texture(stencilTexture, clamp(TexCoords + offset, 0, 1)).r;
        
        outlinemask = mix(outlinemask, 1.0, col);
    }
    outlinemask = mix(outlinemask, 0.0, stencil);

    FragColor = mix(color, vec4(0.75, 0.4, 0.0, 1.0), clamp(outlinemask, 0.0, 1.0));
}
/*
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D image;
  
uniform bool horizontal;
uniform float weight[5] = float[] (0.16522441005671412, 0.2163960891171625, 0.23675900165224684, 0.2163960891171625, 0.16522441005671412);

void main()
{             
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
*/