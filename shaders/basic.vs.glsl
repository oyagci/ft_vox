#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewProjectionMatrix;

out vec3 v_normal;
out vec3 v_fragpos;

void main()
{
	v_normal = (modelMatrix * vec4(in_normal, 0.0)).xyz;
	v_fragpos = (modelMatrix * vec4(in_position, 1.0)).xyz;
	gl_Position = viewProjectionMatrix * modelMatrix * vec4(in_position, 1.0);
}
