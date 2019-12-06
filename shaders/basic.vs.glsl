#version 330 core

layout (location = 0) in vec3 in_position;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 viewProjectionMatrix;
uniform mat4 modelMatrix;

out vec3 FragPos;

void main()
{
	gl_Position = viewProjectionMatrix * modelMatrix * vec4(in_position, 1.0);
	FragPos = vec3(modelMatrix * vec4(in_position, 1.0));
}
