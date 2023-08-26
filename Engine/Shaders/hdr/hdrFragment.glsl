#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{             
    float expo = 0.75f;
    float gamma = 1.0f;
    // Fetch the HDR color from the texture
    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

    // Apply exposure adjustment
    vec3 adjustedColor = hdrColor * expo;

    // Apply gamma correction
    adjustedColor = pow(adjustedColor, vec3(1.0 / gamma));

    // Output the final color
    FragColor = vec4(adjustedColor, 1.0);
}