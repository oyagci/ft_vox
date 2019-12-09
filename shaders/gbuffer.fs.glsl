#version 330 core

layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo;

in vec3 fragpos;
in vec3 v_normal;

void main()
{
	g_position = fragpos;
	g_normal = normalize(v_normal);
	g_albedo = vec4(1, 0, 1, 1);
}
