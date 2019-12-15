#pragma once

#include <memory>
#include "Chunk.hpp"
#include <glm/vec3.hpp>
#include "IRenderer.hpp"
#include "ChunkBuilder.hpp"

class ChunkRenderer : public IRenderer
{
public:
	ChunkRenderer();

	void render() override;
	void addChunk(std::shared_ptr<Chunk> chunk);

private:
	std::vector<ChunkBuilder> _chunks;
	std::vector<Mesh> _meshes;
};
