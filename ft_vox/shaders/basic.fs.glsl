#version 330 core

in vec3 FragPos;
in vec2 TexCoords;

out vec4 frag_color;

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

	float f = calcFog(fogStart, fogStart + 128.0);
	vec3 fogColor = vec3(0.9, 0.9, 1.0);

	// Add fog to final color
	finalColor = vec4((1.0 - f) * fogColor + f * finalColor.xyz, 1.0);

	frag_color = finalColor;
}
