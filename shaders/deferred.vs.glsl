#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

out vec3 fragpos;
out vec3 v_normal;

void main()
{
    v_normal = (modelMatrix * vec4(in_normal, 0.0)).xyz;
    fragpos = (modelMatrix * vec4(in_position, 1.0)).xyz;
	gl_Position = viewProjectionMatrix * modelMatrix * vec4(in_position, 1.0);
}