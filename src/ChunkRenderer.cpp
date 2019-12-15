#include "ChunkRenderer.hpp"

ChunkRenderer::ChunkRenderer()
{
}

void ChunkRenderer::addChunk(std::shared_ptr<Chunk> chunk)
{
	ChunkBuilder ci(chunk);

	_chunks.push_back(std::move(ci));
}

void ChunkRenderer::render()
{
	for (auto &c : _chunks) {
		c.render();
	}
}
