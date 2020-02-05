#pragma once
#include <memory>
#include "WorldGenerator.hpp"
#include "IRenderer.hpp"
#include <list>
#include <vector>
#include <lazy.hpp>
#include "ChunkRenderer.hpp"

using lazy::graphics::Camera;
using lazy::graphics::Shader;

class World : public IRenderer
{
public:
	World(Camera &cam);

	void render() override;
	void update();
	std::optional<Chunk::Block> getBlock(int x, int y, int z);
	std::optional<std::shared_ptr<Chunk>> getChunk(glm::ivec2 pos);

private:
	std::vector<glm::vec2> getChunksTooFar(glm::vec3 camPos);
	void removeChunksTooFar(std::vector<glm::vec2> chunksTooFar);

private:
	std::unique_ptr<ChunkRenderer> _chunkRenderer;
	std::unique_ptr<Shader> _shader;
	std::unique_ptr<WorldGenerator> _generator;

	std::list<std::shared_ptr<Chunk>> _chunks;

	Camera &_camera;

	std::mutex _chunksLock;
};
