#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 tex_coords;

out vec2 TexCoords;

void main()
{
	gl_Position = vec4(in_position.xy, 0.0f, 1.0f);
	TexCoords = tex_coords;
}