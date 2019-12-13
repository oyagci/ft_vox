#pragma once

#include <thread>
#include <atomic>
#include "lazy.hpp"
#include "ChunkFactory.hpp"
#include "ChunkRenderer.hpp"
#include "Chunk.hpp"
#include "IRenderer.hpp"

using namespace lazy::graphics;

class WorldRenderer : public IRenderer
{
public:
	WorldRenderer(Camera &camera, glm::vec3 &camPos);
	~WorldRenderer();

	void generateChunks();
	void render() override;
	void setPos(glm::vec3 pos);

private:
	const int RENDER_DISTANCE = 5;

	glm::vec3 &_camPos;
	std::unique_ptr<Shader> _shader;
	std::unique_ptr<ChunkFactory> _factory;
	std::unique_ptr<ChunkRenderer> _renderer;
	std::vector<std::shared_ptr<Chunk>> _chunks;
	Camera &_camera;

	std::vector<glm::vec3> _chunksToGenerate;
	std::thread _workerThread;
	std::atomic_bool _isWorking;
	std::atomic_bool _shouldJoin;
};
