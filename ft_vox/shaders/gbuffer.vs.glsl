#version 330 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 tex_coords;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

void main()
{
	vec4 viewPos = viewMatrix * modelMatrix * vec4(in_position, 1.0);

	FragPos = viewPos.xyz;
	TexCoords = tex_coords;
	Normal = mat3(transpose(inverse(viewMatrix * modelMatrix))) * in_normal;

	gl_Position = projectionMatrix * viewPos;
}
