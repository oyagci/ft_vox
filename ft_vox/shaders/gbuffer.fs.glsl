#version 330 core

layout (location = 0) out vec4 gColor;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D tex;
uniform vec3 cameraPosition;
uniform float fogStart;

float calcFog(float fogStart, float fogEnd)
{
	float f = (fogEnd - (gl_FragCoord.z / gl_FragCoord.w)) / (fogEnd - fogStart);

	return clamp(f, 0.0, 1.0);
}

void main()
{
	vec4 finalColor = texture(tex, TexCoords);

	//float f = calcFog(fogStart, fogStart + 128.0);
	//vec3 fogColor = vec3(0.9, 0.9, 1.0);

	//// Add fog to final color
	//finalColor = vec4((1.0 - f) * fogColor + f * finalColor.xyz, 1.0);

	gColor = finalColor;
	gNormal = vec4(Normal, 1.0);
	gPosition = vec4(FragPos, 1.0);
}
