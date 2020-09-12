#pragma once

#include "lazy.hpp"

class DebugGrid
{
private:
	GLuint _Vao;
	GLuint _Vbo;
	lazy::graphics::Shader _Shader;

	DebugGrid();

public:

	DebugGrid(DebugGrid const &) = delete;
	DebugGrid &operator=(DebugGrid const &) = delete;

	static auto &get()
	{
		static DebugGrid instance;
		return instance;
	}

	void draw(glm::vec3 const &position = { 0, 0, 0 }, float size = 500.0f, float borderSize = 0.03f);
};
