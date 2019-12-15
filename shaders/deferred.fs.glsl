#version 330 core

out vec4 frag_color;

in vec3 v_position;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D albedo;

void main()
{
	vec2 quad_texcoord = v_position.xy * 0.5 + 0.5;
	vec3 position = texture(position, quad_texcoord).xyz;
	vec3 normal = texture(normal, quad_texcoord).xyz;
	vec3 albedo = texture(albedo, quad_texcoord).xyz;

	float light = dot(normalize(position - vec3(0, 0, -5)), normal);
	frag_color = vec4(vec3(light), 1);

	// frag_color = vec4(0, 0, 0, 1);
	// frag_color += texture(position, v_position.xy);
	// frag_color += texture(normal, vec2(v_position.x + 1, v_position.y));
	// frag_color += texture(albedo, vec2(v_position.x + 1, v_position.y + 1));
}
