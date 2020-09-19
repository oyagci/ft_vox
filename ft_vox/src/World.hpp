#pragma once
#include <memory>
#include "WorldGenerator.hpp"
#include <list>
#include <vector>
#include <lazy.hpp>
#include "ChunkRenderer.hpp"
#include "Cubemap.hpp"
#include <optional>
#include "Region.hpp"

using lazy::graphics::Camera;
using lazy::graphics::Shader;

class World
{
public:
	World(Camera &cam);

	void render(Shader &shader);
	void update();
	std::optional<Chunk::Block> getBlock(int x, int y, int z);
	std::optional<std::shared_ptr<ChunkController>> getChunk(glm::ivec2 pos);

	glm::vec3 getPlayerPosition() const { return _camera.getPosition(); }
	auto &getCamera() const { return _camera; }
	auto &getChunks() const { return _chunksRegion; }

	void setBlock(glm::ivec3 const &position, Chunk::Block value);

private:
	std::vector<glm::vec2> getChunksTooFar(glm::vec3 camPos);
	void removeChunksTooFar(std::vector<glm::vec2> chunksTooFar);

private:
	std::unique_ptr<ChunkRenderer> _chunkRenderer;
	std::unique_ptr<WorldGenerator> _generator;
	std::unique_ptr<Cubemap> _cubemap;
	std::unique_ptr<Shader> _cubemapShader;

	std::list<std::shared_ptr<ChunkController>> _chunks;

	Region<std::shared_ptr<ChunkController>> _chunksRegion;

	Camera &_camera;

	std::mutex _chunksLock;
};
