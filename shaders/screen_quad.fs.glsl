#version 330 core

out vec4 frag_color;

in vec3 v_position;

uniform sampler2D tex;

void main()
{
	frag_color = texture(tex, v_position.xy * 0.5 + 0.5);
}
