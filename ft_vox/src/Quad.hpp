#pragma once

#include "lazy.hpp"

class Quad
{
private:
	lazy::graphics::Mesh _quad;

	void initQuad()
	{
		std::array<glm::vec3, 4> pos = {
			glm::vec3(-1.0f, -1.0f, 0.0f),
			glm::vec3( 1.0f, -1.0f, 0.0f),
			glm::vec3( 1.0f,  1.0f, 0.0f),
			glm::vec3(-1.0f,  1.0f, 0.0f),
		};
		std::array<glm::vec2, 4> uv = {
			glm::vec2(0.0f,  0.0f),
			glm::vec2(1.0f,  0.0f),
			glm::vec2(1.0f,  1.0f),
			glm::vec2(0.0f,  1.0f),
		};

		for (auto p : pos) {
			_quad.addPosition(p);
		}

		for (auto &u : uv) {
			_quad.addUv(u);
		}
		_quad.addTriangle(glm::uvec3(0, 1, 2));
		_quad.addTriangle(glm::uvec3(0, 2, 3));

		_quad.build();
	}

public:
	Quad()
	{
		initQuad();
	}

	void draw()
	{
		_quad.draw();
	}
};
