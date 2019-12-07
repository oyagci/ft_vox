#pragma once

#include "lazy.hpp"
#include "ChunkFactory.hpp"
#include "ChunkRenderer.hpp"
#include "Chunk.hpp"

using namespace lazy::graphics;

class WorldRenderer
{
public:
	WorldRenderer(Camera &camera, glm::vec3 &camPos);

	void generateChunks();
	void renderChunks();
	void setPos(glm::vec3 pos);

private:
	glm::vec3 &_camPos;
	std::unique_ptr<Shader> _shader;
	std::unique_ptr<ChunkFactory> _factory;
	std::unique_ptr<ChunkRenderer> _renderer;
	std::vector<std::shared_ptr<Chunk>> _chunks;
	Camera &_camera;
};
