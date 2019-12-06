#pragma once

#include <lazy.hpp>

using namespace lazy::graphics;

class TextRenderer
{
public:
	TextRenderer();

	void drawText(std::string text, glm::vec2 pos, GLfloat scale, glm::vec3 color);

private:
	GLuint _vao;
	GLuint _vbo;
	Shader _shader;

	struct Character {
		GLuint texture;
		glm::ivec2 size;
		glm::ivec2 bearing;
		GLuint advance;
	};
	std::map<GLchar, Character> _characters;

};
