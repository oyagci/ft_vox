#version 330 core
#include "lights.glsl"

out vec4 frag_color;

in vec3 v_position;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D albedo;

uniform PointLight light;

void main()
{
	vec2 quad_texcoord = v_position.xy * 0.5 + 0.5;
	vec3 position = texture(position, quad_texcoord).xyz;
	vec3 normal = texture(normal, quad_texcoord).xyz;
	vec3 albedo = texture(albedo, quad_texcoord).xyz;

	float lightDot = dot(normalize(position - light.position), -normal);
    vec4 lightFinal = lightDot * light.color;
	frag_color = lightFinal;
}
