#pragma once

#include <memory>
#include "Chunk.hpp"
#include <glm/vec3.hpp>
#include "IRenderer.hpp"
#include "ChunkBuilder.hpp"
#include "threadpool/threadpool.hpp"
#include <queue>
#include <tuple>

struct ChunkMesh {
	Mesh mesh;
	bool isBuilt;

	ChunkMesh(Mesh mesh, bool isBuilt = false) : mesh(std::move(mesh)), isBuilt(isBuilt) {}
	void build() {
		if (!isBuilt) {
			mesh.build();
			isBuilt = true;
		}
	}
	void draw() {
		mesh.draw();
	}
};

class ChunkRenderer : public IRenderer
{
public:
	ChunkRenderer();

	void render() override;
	void addChunk(std::shared_ptr<Chunk> chunk);
	void update();

private:
	void buildChunks();

private:
	std::unique_ptr<ChunkBuilder> _builder;

	std::vector<ChunkMesh> _meshes;
	std::vector<std::shared_ptr<Chunk>> _chunks;

	using Faces = std::vector<ChunkBuilder::Face>;

	std::mutex _cf;
	std::queue<std::tuple<glm::vec2, Faces>> _chunkFaces;

	thread_pool _pool;

	std::mutex _cm;
	std::queue<ChunkMesh> _chunkMeshes;
};
