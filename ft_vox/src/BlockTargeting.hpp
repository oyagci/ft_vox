#pragma once

#include <glm/vec3.hpp>
#include "AxisAlignedBoundingBox.hpp"
#include "octree/Octree.hpp"
#include "lazy.hpp"

struct Ray
{
	glm::vec3 Origin;
	glm::vec3 Direction;
};

struct HitResult
{
	float Near;
	float Far;
};

class TargetBlockMesh
{
private:
	GLuint _Vao;
	GLuint _Vbo;
	lazy::graphics::Shader _Shader;

public:
	TargetBlockMesh()
	{
		static const float vertices[] = {
			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,

			0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
			0.0f, 0.0f, 1.0f,

			0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 1.0f,

			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 1.0f,

			0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 0.0f,

			0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &_Vao);
		glBindVertexArray(_Vao);

		glGenBuffers(1, &_Vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _Vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);

		glBindVertexArray(0);

		_Shader.addVertexShader("shaders/highlight.vs.glsl")
			.addFragmentShader("shaders/highlight.fs.glsl")
			.link();
	}

	~TargetBlockMesh()
	{
		glDeleteBuffers(1, &_Vbo);
		glDeleteVertexArrays(1, &_Vao);
	}

	void draw(glm::vec3 position);
};

class BlockTargeting
{
private:
	HitResult raycast(AxisAlignedBoundingBox const &aabb, Ray const &ray);
	HitResult raycastAgainstOctree(octree::Octree const &octree, Ray const &ray);

	TargetBlockMesh _targetBlockHighlight;

	glm::vec3 _targetBlockPosition;

public:
	BlockTargeting();

	void update(glm::vec3 const &position, glm::vec3 const &direction);
	void draw() { _targetBlockHighlight.draw(_targetBlockPosition); }
};
