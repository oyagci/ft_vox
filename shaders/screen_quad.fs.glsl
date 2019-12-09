#version 330 core

out vec4 frag_color;

in vec3 v_position;

uniform sampler2D position;
uniform sampler2D normal;
uniform sampler2D albedo;

void main()
{
	frag_color = vec4(0, 0, 0, 1);
	frag_color += texture(position, v_position.xy);
	frag_color += texture(normal, vec2(v_position.x + 1, v_position.y));
	frag_color += texture(albedo, vec2(v_position.x + 1, v_position.y + 1));
}
