#version 430

layout(rgba32f, binding = 0) uniform image2D scene;
layout(rgba32f, binding = 1) uniform image2D blurred;
layout(rgba32f, binding = 2) uniform image2D outImage;

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
void main() {
    ivec2 texelCoords = ivec2(gl_GlobalInvocationID.xy);
    vec4 color = (imageLoad(scene, texelCoords));
    if(color.x + color.y + color.z >= 3.0)
        color.xyz = vec3(1.0f);
    else
     color += imageLoad(blurred, texelCoords);//(imageLoad(blurred, texelCoords) * 2);

    imageStore(outImage, texelCoords, color);
}
