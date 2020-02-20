#version 450 core

in vec2 TexCoords;
out vec4 frag_color;

uniform sampler2D tilemap;

void main()
{
	vec2 tile = vec2(2.0f / 16.0f, 15.0f / 16.0f);
	vec2 size = vec2(1.0 / 16.0f, 1.0f /16.0f);
	vec2 texCoord = tile + size * fract(TexCoords);
	frag_color = texture(tilemap, texCoord);
}
