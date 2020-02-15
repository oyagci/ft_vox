#version 330 core
#include "lights.glsl"

out vec4 frag_color;

in vec3 v_position;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D albedo;
uniform sampler2D depth;

uniform DirectionalLight light;

void main()
{
	vec2 quad_texcoord = v_position.xy * 0.5 + 0.5;
	vec4 fragPos = texture(position, quad_texcoord);
	vec3 normal = texture(normal, quad_texcoord).xyz;
	vec3 albedo = texture(albedo, quad_texcoord).xyz;
	float depth = texture(depth, quad_texcoord).x;

	vec4 finalLight = calcDirectionalLight(light, normal, fragPos.xyz);

	if (light.light.hasShadow == 1)
	{
		finalLight *= calcBlurredShadowFactor(light.light.shadowMap, light.light.shadowProjection, fragPos, 3, 20);
	}

	frag_color = finalLight;
}
