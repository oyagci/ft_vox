#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 modelMatrix;
uniform mat4 viewProjectionMatrix;

void main()
{
	gl_Position = viewProjectionMatrix * modelMatrix * vec4(in_position, 1.0);
}
