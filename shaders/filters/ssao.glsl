#version 330 core

out vec4 frag_color;

in vec3 v_position;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D albedo;
uniform sampler2D depth;

uniform sampler2D ssaoNoise;
uniform vec3 ssaoKernel[64];

uniform int kernelSize;

uniform float screen_w;
uniform float screen_h;

uniform vec3 cameraPosition;
uniform vec3 cameraDirection;

uniform mat4 projectionMatrix;
uniform mat4 inverseProjectionMatrix;
uniform mat4 viewMatrix;

#define CAP_MIN_DISTANCE 0.0001
#define CAP_MAX_DISTANCE 0.0025


void main()
{
    vec2 noiseScale = vec2(screen_w / 4.0, screen_h / 4.0);
	vec2 v_texCoord = v_position.xy * 0.5 + 0.5;
    float radius = 1.0f;

	vec4 posView = (viewMatrix * texture(position, v_texCoord));
	
	vec3 normalView = normalize((texture(normal, v_texCoord)).xyz);
	
	// frag_color = vec4(normalView, 1.0);
	// return; 
	vec3 randomVector = normalize(texture(ssaoNoise, v_texCoord * noiseScale).xyz * 2.0 - 1.0);
	vec3 tangentView = normalize(randomVector - dot(randomVector, normalView) * normalView);
	
	vec3 bitangentView = cross(normalView, tangentView);
	mat3 kernelMatrix = mat3(tangentView, bitangentView, normalView); 

	float occlusion = 0.0;
	
	for (int i = 0; i < kernelSize; i++)
	{
		vec3 sampleVectorView = kernelMatrix * ssaoKernel[i];
		vec4 samplePointView = vec4(posView.xyz + radius * sampleVectorView, 1.0);
		vec4 samplePointNDC = projectionMatrix * samplePointView;
		samplePointNDC /= samplePointNDC.w;
		vec2 samplePointTexCoord = samplePointNDC.xy * 0.5 + 0.5;   
		float zSceneNDC = texture(depth, samplePointTexCoord).r * 2.0 - 1.0;
		
		float bias = 0.025;
		// float bias = 0.0;
        // occlusion += (zSceneNDC >= samplePointNDC.z ? 0.0 : 1.0);
		// occlusion += (zSceneNDC >= samplePointNDC.z ? 0.0 : 1.0); 

		float rangeCheck = samplePointNDC.z;
		// frag_color = vec4((zSceneNDC >= samplePointNDC.z ? 0.0 : 1.0));
		// return;
		occlusion       += (zSceneNDC >= samplePointNDC.z ? 0.0 : 1.0) * rangeCheck;  	
	}
	
	occlusion = 1.0 - (occlusion / kernelSize);
	frag_color = vec4(occlusion * 2.0);
}

// void main()
// {
//     vec2 noiseScale = vec2(screen_w / 4.0, screen_h / 4.0);
// 	vec2 quad_texcoord = v_position.xy * 0.5 + 0.5;
// 	// vec3 fragPos = (viewMatrix * texture(position, quad_texcoord)).xyz;
// 	vec3 fragPos = getViewPos(quad_texcoord).xyz;
// 	vec3 normal = normalize((texture(normal, quad_texcoord)).xyz);
// 	vec3 albedo = texture(albedo, quad_texcoord).xyz;
// 	// float depthTexture = texture(depth, quad_texcoord).x;

// 	// // vec3 randomVec = texture(ssaoNoise, quad_texcoord * noiseScale).xyz;

//     // frag_color = vec4(normal, 1.0);
//     // return;
//     // vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
//     // vec3 bitangent = cross(normal, tangent);
//     // mat3 TBN = mat3(tangent, bitangent, normal);  

//     vec3 rvec = texture(ssaoNoise, quad_texcoord * noiseScale).xyz * 2.0 - 1.0;
//     vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
//     vec3 bitangent = cross(normal, tangent);
//     mat3 TBN = mat3(tangent, bitangent, normal);

//     float radius = 1.0;

//     float occlusion = 0.0;
//     for(int i = 0; i < kernelSize; ++i)
//     {
//         // get sample position
//         vec3 sample = TBN * vec3(0, 1, 0);// * getKernelSample(i); // From tangent to view-space
//         sample = sample * radius + fragPos; 

//         vec4 offset = vec4(sample, 1.0);
//         offset = projectionMatrix * offset;
//         offset.xyz /= offset.w;
//         offset.xyz = offset.xyz * 0.5 + 0.5;

//         // vec3 toOriginal = fragPos - cameraPosition;
//         // float sampleDepth = dot(cameraDirection, toOriginal);
//         float sampleDepth = texture(depth, offset.xy).x;

//         frag_color = vec4(sampleDepth);
//         return;
//         // frag_color = vec4(sample.z);
//         // return;

//         // float sampleDepth = texture(depth, offset.xy).x;
//         // occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0);

//         float bias = 0.025;
//         float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
//         occlusion       += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck; 

//         // return;
//         // frag_color = vec4(occlusion);
//         // return;

//         // float rangeCheck= abs(fragPos.z - sampleDepth) < radius ? 1.0 : 0.0;
//         // occlusion += (sampleDepth <= sample.z ? 1.0 : 0.0) * rangeCheck;
//     }  

//     occlusion = 1.0 - (occlusion / kernelSize);
//     frag_color = vec4(occlusion); 
// }
