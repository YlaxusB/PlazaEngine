#version 460 core

uniform sampler2D gFinalImage;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gOthers;
uniform sampler2D gDepth;
uniform sampler2D sceneColor; 

uniform mat4 invView;
uniform mat4 projection;
uniform mat4 invprojection;
uniform mat4 view;
//uniform vec3 viewPos;

/*
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gOthers;
uniform sampler2D gDepth;

uniform sampler2D sceneColor;

uniform vec3 viewPos;
uniform mat4 view;
uniform mat4 invProjection;
uniform mat4 projection;
*/

noperspective in vec2 TexCoords;

out vec4 outColor;

const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const int numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;

float Metallic;

#define Scale vec3(.8, .8, .8)
#define K 19.19

vec3 PositionFromDepth(float depth);

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth);
 
vec4 RayCast(vec3 dir, inout vec3 hitCoord, out float dDepth);

vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth);

vec3 fresnelSchlick(float cosTheta, vec3 F0);

vec3 hash(vec3 a);

void main()
{

    vec2 MetallicEmmissive = vec2(1.0f);//texture2D(gOthers, TexCoords).rg;
    Metallic = MetallicEmmissive.r;

    if(Metallic < 0.01)
    {
        outColor = texture(sceneColor, TexCoords);
        return;
    }
    
    //if(texture(gDepth, TexCoords).x < 0.00001f)
    //{
    //    outColor = texture(sceneColor, TexCoords);
    //    return;
    //}
 
    vec3 viewNormal = vec3(texture2D(gNormal, TexCoords) * invView);
    vec3 viewPos = texture(gPosition, TexCoords).xyz;
    vec3 albedo = texture(sceneColor, TexCoords).rgb;

    float spec = 1.0f;//texture(gOthers, TexCoords).r;

    vec3 F0 = vec3(0.04); 
    F0      = mix(F0, albedo, Metallic);
    F0 = texture(gOthers, TexCoords).rgb;
    vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);

    // Reflection vector
    vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));


    vec3 hitPos = viewPos;
    float dDepth;
 
    vec3 wp = vec3(vec4(viewPos, 1.0) * invView);
    vec3 jitt = mix(vec3(0.0), vec3(hash(wp)), spec);
    vec4 coords = RayMarch((vec3(jitt) + reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
 
 
    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
 
 
    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

    float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) * 
                screenEdgefactor * 
                -reflected.z;
 
    // Get color
    vec3 SSR = texture(sceneColor, coords.xy).rgb * clamp(ReflectionMultiplier, 0.0, 0.9) * Fresnel;  
    //outColor = vec4(1.0f);

    outColor = vec4(texture(sceneColor, TexCoords.xy).rgb + SSR, 1.0f);
    //outColor = vec4(vec3(texture(gDepth, TexCoords).x), 1.0f);

    //vec2 re = TexCoords * vec2(1820, 720);
    //if(re.x >= 850 && re.x <= 950 && re.y >= 200 && re.y <= 300)
    //{
    //    if(hitPos.z == 0.50)
    //        outColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    //    else
    //        outColor = vec4(hitPos, 1.0f);
    //}
}

vec3 PositionFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invprojection * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
    float depth;

    vec4 projectedCoord;
 
    for(int i = 0; i < numBinarySearchSteps; i++)
    {

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = texture(gPosition, projectedCoord.xy).z;

 
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
    }

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec3(projectedCoord.xy, depth);
}

vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{

    dir *= step;
 
 
    float depth;
    int steps;
    vec4 projectedCoord;

 
    for(int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;
 
        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = texture2D(gPosition, projectedCoord.xy).z;//texture(gDepth, projectedCoord.xy).z;//texture(gOthers, TexCoords).z;//textureLod(gPosition, projectedCoord.xy, 2).z;
        if(depth > 1000.0)
            continue;
  
        dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2)
        {
            if(dDepth <= 0.0)
            {   
                vec4 Result;
                Result = vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);

                return Result;
            }
        }
        
        steps++;
    }
 
    
    return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 hash(vec3 a)
{
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx)*a.zyx);
}

/*
#version 460 core
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gDiffuse;
uniform sampler2D gOthers;
uniform sampler2D gDepth;

uniform sampler2D sceneColor;

uniform vec3 viewPos;
uniform mat4 view;
uniform mat4 invProjection;
uniform mat4 projection;

out vec4 FragColor;

bool rayIsOutofScreen(vec2 ray){
	return (ray.x > 1 || ray.y > 1 || ray.x < 0 || ray.y < 0) ? true : false;
}

vec3 TraceRay(vec3 rayPos, vec3 dir, int iterationCount){
	float sampleDepth;
	vec3 hitColor = texture(sceneColor, TexCoords).rgb;//vec3(0);
	bool hit = false;

	for(int i = 0; i < iterationCount; i++){
		rayPos += dir;
		if(rayIsOutofScreen(rayPos.xy)){
			break;
		}

		sampleDepth = texture(gDepth, rayPos.xy).z;
		float depthDif = rayPos.z - sampleDepth;
		if(depthDif >= 0 && depthDif < 0.00001){ //we have a hit
			hit = true;
			hitColor += texture(sceneColor, rayPos.xy).rgb;
			break;
		}
	}
	return hitColor;
}

#define SCR_WIDTH 1820
#define SCR_HEIGHT 720


void main()
{       
	float maxRayDistance = 100.0f;

	//View Space ray calculation
	vec3 pixelPositionTexture;
	pixelPositionTexture.xy = vec2(gl_FragCoord.x / SCR_WIDTH,  gl_FragCoord.y / SCR_HEIGHT);
	vec3 normalView = texture(gNormal, pixelPositionTexture.xy).rgb;	
	float pixelDepth = texture(gDepth, pixelPositionTexture.xy).z;	// 0< <1
	pixelPositionTexture.z = pixelDepth;		
	vec4 positionView = invProjection * vec4(pixelPositionTexture * 2 - vec3(1), 1);
	positionView /= positionView.w;
	vec3 reflectionView = normalize(reflect(positionView.xyz, normalView));
	if(reflectionView.z > 0){
		FragColor = vec4(texture(sceneColor, TexCoords).rgb, 1.0f);
		//FragColor = vec4(0,0,0,1);
		return;
	}
	vec3 rayEndPositionView = positionView.xyz + reflectionView * maxRayDistance;


	//Texture Space ray calculation
	vec4 rayEndPositionTexture = projection * vec4(rayEndPositionView,1);
	rayEndPositionTexture /= rayEndPositionTexture.w;
	rayEndPositionTexture.xyz = (rayEndPositionTexture.xyz + vec3(1)) / 2.0f;
	vec3 rayDirectionTexture = rayEndPositionTexture.xyz - pixelPositionTexture;

	ivec2 screenSpaceStartPosition = ivec2(pixelPositionTexture.x * SCR_WIDTH, pixelPositionTexture.y * SCR_HEIGHT); 
	ivec2 screenSpaceEndPosition = ivec2(rayEndPositionTexture.x * SCR_WIDTH, rayEndPositionTexture.y * SCR_HEIGHT); 
	ivec2 screenSpaceDistance = screenSpaceEndPosition - screenSpaceStartPosition;
	int screenSpaceMaxDistance = max(abs(screenSpaceDistance.x), abs(screenSpaceDistance.y)) / 2;
	rayDirectionTexture /= max(screenSpaceMaxDistance, 0.001f);


	//trace the ray
	vec3 outColor = TraceRay(pixelPositionTexture, rayDirectionTexture, screenSpaceMaxDistance);
	FragColor = vec4( reflectionView.z < 0 ? outColor : vec3(0.0f), 1);
}

*/