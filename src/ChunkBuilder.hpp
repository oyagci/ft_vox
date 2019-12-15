#pragma once

#include <glm/vec3.hpp>
#include <memory>
#include "IRenderer.hpp"
#include "Chunk.hpp"

class ChunkBuilder : public IRenderer
{
private:
	enum FaceDirection {
		FD_TOP,
		FD_BOT,
		FD_LEFT,
		FD_RIGHT,
		FD_FRONT,
		FD_BACK,
	};
	struct Face {
		glm::vec3 pos;
		FaceDirection dir;
	};

public:
	ChunkBuilder(std::shared_ptr<Chunk> chunk);

	const auto &getChunk() const { return _chunk; }

	void render() override;

private:
	void update();
	void updateChunk();

	void addFaceToRender(glm::vec3 pos, FaceDirection f);
	void buildChunkMesh();
	void buildTopFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildFrontFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBotFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildBackFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildRightFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);
	void buildLeftFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset);

	int getVisibleFaces(int x, int y, int z);

	bool _hasChanged;

	std::shared_ptr<Chunk> _chunk;
	std::vector<Face> _faces;
	Mesh _mesh;
};
