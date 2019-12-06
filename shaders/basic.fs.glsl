#version 330 core

in vec3 FragPos;

out vec4 frag_color;

void main()
{
	frag_color = vec4(sin(FragPos.x), sin(FragPos.y), sin(FragPos.z), 1);
}
