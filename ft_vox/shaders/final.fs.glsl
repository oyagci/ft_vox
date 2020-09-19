#version 450 core

out vec4 frag_color;

in vec2 TexCoords;

uniform sampler2D colorTexture;
uniform sampler2D positionTexture;
uniform sampler2D depthTexture;
uniform sampler2D normalTexture;
uniform sampler2D ssaoTexture;

uniform mat4 projectionMatrix;

uniform float fogStart = 128.0;
uniform vec3 fogColor = vec3(0.9, 0.9, 1.0);

float calc_fog(float fogStart, float fogEnd)
{
	vec3 fragPos = texture(positionTexture, TexCoords).xyz;
	vec4 clipSpace = projectionMatrix * vec4(fragPos, 1.0);

	float f = (fogEnd - (clipSpace.z)) / (fogEnd - fogStart);

	return clamp(f, 0.0, 1.0);
}

void main()
{
	vec3 color = texture(colorTexture, TexCoords).rgb;

	color *= texture(ssaoTexture, TexCoords).r;

	float fogAmount = calc_fog(fogStart, fogStart + 128.0);
	color = (1.0 - fogAmount) * fogColor + fogAmount * color;

	frag_color = vec4(color, 1.0f);
}
