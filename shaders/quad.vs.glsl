#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 modelViewMatrix;

out vec3 v_position;
out vec2 v_texcoord;

void main()
{
	v_position = in_position;
	v_texcoord = in_position.xy * 0.5 + 0.5;
	gl_Position = modelViewMatrix * vec4(in_position, 1.0);
}