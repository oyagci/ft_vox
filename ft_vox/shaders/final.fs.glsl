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

	const vec2 size = textureSize(colorTexture, 0);
	const float crosshairLength = 8.0;
	const float crosshairWidth = 1.0;

	// This will be either 0 or 1
	int crosshair = int(
	   (TexCoords.x * size.x >= (size.x / 2.0) - crosshairLength &&
		TexCoords.x * size.x <= (size.x / 2.0) + crosshairLength &&
		TexCoords.y * size.y >= (size.y / 2.0) - crosshairWidth &&
		TexCoords.y * size.y <= (size.y / 2.0) + crosshairWidth)
			||
	   (TexCoords.x * size.x >= (size.x / 2.0) - crosshairWidth &&
		TexCoords.x * size.x <= (size.x / 2.0) + crosshairWidth &&
		TexCoords.y * size.y >= (size.y / 2.0) - crosshairLength &&
		TexCoords.y * size.y <= (size.y / 2.0) + crosshairLength)
	);

	// Branchless condition:
	// If crosshair == 1 then set crosshair color
	// Else keep original color
	color = (crosshair * (1.0 - color + 0.1)) +
			(1.0 - crosshair) * color;

	frag_color = vec4(color, 1.0f);
}
