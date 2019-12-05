#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

uniform vec3 offsets[100];

void main()
{
	vec3 off = offsets[gl_InstanceID];

	gl_Position = viewProjectionMatrix * modelMatrix * vec4(in_position + off, 1.0);
}
