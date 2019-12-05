#pragma once

#include <memory>
#include "Chunk.hpp"
#include <glm/vec3.hpp>

class ChunkRenderer
{
private:
	struct Faces
	{
		// Faces to be rendered
		bool top;
		bool bot;
		bool left;
		bool right;
		bool front;
		bool back;

		// Chunk-local position
		glm::vec3 pos;

		Faces(float x, float y, float z)
			: top(false), bot(false), left(false), right(false), front(false),
			  back(false), pos(x, y, z) {}
	};

public:
	ChunkRenderer();

	void onTessellate();
	void onRender();
	void onUpdateMesh();

	// Setters
	void setChunk(Chunk chunk);

private:
	typedef std::array<glm::vec3, 4> quadVerts;
	typedef std::array<unsigned int, 6> quadInds;
	typedef std::tuple<quadVerts, quadInds> quad;

	quad genTopQuad(glm::vec3 pos);
	quad genBotQuad(glm::vec3 pos);
	quad genLeftQuad(glm::vec3 pos);
	quad genRightQuad(glm::vec3 pos);
	quad genFrontQuad(glm::vec3 pos);
	quad genBackQuad(glm::vec3 pos);

private:
	Chunk _chunk;
	GLuint _vbo;
	GLuint _vao;

	std::vector<Faces> _faces;
	std::vector<quad> _quads;

	Mesh _mesh;
};
