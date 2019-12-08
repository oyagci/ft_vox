#version 330 core

out vec4 frag_color;

in vec3 v_fragpos;
in vec3 v_normal;

void main()
{
	float light = dot(normalize(v_fragpos - vec3(0, 0, -5)), v_normal);
	frag_color = vec4(vec3(light), 1);
}
