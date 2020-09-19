#version 330 core

out float frag_color;

in vec2 TexCoords;

uniform sampler2D positionTexture;
uniform sampler2D normalTexture;
uniform sampler2D ssaoNoise;
uniform vec3      ssaoKernel[64]; 

uniform mat4 projectionMatrix;

float calc_ssao()
{
	const vec2 noiseTextureSize = vec2(4.0, 4.0);
	const vec2 noiseScale = vec2(1920.0 / noiseTextureSize.x, 1080.0 / noiseTextureSize.y);

	vec3 fragPos = texture(positionTexture, TexCoords).xyz;
	vec3 normal = texture(normalTexture, TexCoords).xyz;
	vec3 randomVec = texture(ssaoNoise, TexCoords * noiseScale).xyz;

	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
	vec3 bitangent = cross(normal, tangent);
	mat3 tbn = mat3(tangent, bitangent, normal);

	float occlusion = 0.0;
	int kernelSize = 64;
	float radius = 2.5;
	float bias = 0.025;

	for (int i = 0; i < kernelSize; i++)
	{
		vec3 kernelSample = tbn * ssaoKernel[i];
		kernelSample = fragPos + kernelSample * radius;

		vec4 offset = vec4(kernelSample, 1.0);
		offset      = projectionMatrix * offset;
		offset.xyz /= offset.w;
		offset.xyz  = offset.xyz * 0.5 + 0.5;

		float sampleDepth = texture(positionTexture, offset.xy).z;

		float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));

		occlusion += (sampleDepth >= kernelSample.z + bias ? 1.0 : 0.0) * rangeCheck;
	}

	occlusion = 1.0 - (occlusion / kernelSize);

	return occlusion;
}

void main()
{
	frag_color = calc_ssao();
}