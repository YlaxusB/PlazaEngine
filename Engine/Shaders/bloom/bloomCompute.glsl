#version 430

layout(rgba32f, binding = 0) uniform image2D image1;
layout(rgba32f, binding = 1) uniform image2D image2;
uniform bool readFirst;
uniform bool upscale;
uniform vec2 inputResolution;
uniform vec2 outputResolution;
uniform bool blur;


// Constants
#define KERNEL_SIZE 4

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162) ;

#define SIGMA 0.8

float gaussian(float x, float sigma) {
    return exp(-(x * x) / (2.0 * sigma * sigma)) / (sqrt(2.0 * 3.14159) * sigma);
}


// Gaussian kernel
//const float kernel[KERNEL_SIZE] = float[KERNEL_SIZE](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

vec4 boxBlur(ivec2 TexCoords)
{
    vec4 pixel;

    vec2 tex_offset = 1.0 / gl_NumWorkGroups.xy; // gets size of single texel
    
    vec3 result = vec3(0.0f);
    int halfKernelSize = !upscale ? KERNEL_SIZE / 2 : KERNEL_SIZE;
    if(horizontal)
    {

        for (int i = -halfKernelSize; i <= halfKernelSize; ++i) {
            ivec2 offset = ivec2(i, 0);
            vec4 color = imageLoad(image1, TexCoords + offset).rgba;

            // Gaussian kernel weights (you may use pre-calculated weights)
            float weight = gaussian(float(i), SIGMA);

            // Accumulate weighted color values
            result += weight * color.rgb;
        }
    }
    else
    {

        for (int i = -halfKernelSize; i <= halfKernelSize; ++i) {
            ivec2 offset = ivec2(0, i);
            vec4 color = imageLoad(image1, TexCoords + offset).rgba;

            // Gaussian kernel weights (you may use pre-calculated weights)
            float weight =  gaussian(float(i), SIGMA);

            // Accumulate weighted color values
            result += weight * color.rgb;
        }
    }
    return vec4(result, 1.0f);
}

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
    ivec2 texelCoords = ivec2(gl_GlobalInvocationID.xy);

    if(horizontal)
    {

        if(!upscale)
        {
            uvec2 outputCoords = uvec2(gl_GlobalInvocationID.xy);
            uvec2 inputCoords = uvec2(
                    float(outputCoords.x) / float(outputResolution.x) * float(inputResolution.x),
                    float(outputCoords.y) / float(outputResolution.y) * float(inputResolution.y)
                );

            // Blur image1 and store the result in image2
            vec2 asd = (texelCoords / vec2(inputResolution)) * vec2(outputResolution);
            ivec2 correctedTexelCoords = ivec2(asd);


            imageStore(image2, ivec2(outputCoords), boxBlur(ivec2(inputCoords )));
        } 
        else
        {
            // Upscaling operation: Interpolate values from the smaller texture to the larger one
            vec2 asd = (texelCoords / vec2(inputResolution)) * vec2(outputResolution);
            ivec2 correctedTexelCoords = ivec2(asd);

            
            ivec2 outputCoords = ivec2(gl_GlobalInvocationID.xy);
            ivec2 inputCoords = ivec2(
                float(texelCoords.x) / float(gl_NumWorkGroups.x) * float(inputResolution.x),
                float(texelCoords.y) / float(gl_NumWorkGroups.y) * float(inputResolution.y)
            );

            imageStore(image2, ivec2(outputCoords), boxBlur(inputCoords));
        }
    }
    else
    {
        imageStore(image2, ivec2(texelCoords), boxBlur(texelCoords));
    }
}
