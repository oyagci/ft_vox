#version 330 core

layout (location = 0) in vec3 in_position;

uniform vec3 u_viewPosition;
uniform vec3 u_viewDirection;

out vec3 v_position;

out vec3 viewPosition;
out vec3 viewDirection;

void main()
{
	viewPosition = u_viewPosition;
	viewDirection = u_viewDirection;

	v_position = in_position;
	gl_Position = vec4(in_position, 1.0);
}