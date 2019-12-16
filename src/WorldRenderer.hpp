#pragma once

#include <thread>
#include <atomic>
#include "lazy.hpp"
#include "ChunkFactory.hpp"
#include "ChunkRenderer.hpp"
#include "Chunk.hpp"
#include "IRenderer.hpp"
#include "threadpool/threadpool.hpp"
#include "threadpool/blocking_queue.hpp"
#include <queue>

using namespace lazy::graphics;

class WorldRenderer : public IRenderer
{
public:
	WorldRenderer(Camera &camera, glm::vec3 &camPos);
	~WorldRenderer();

	void generateChunks();
	void render() override;
	void update();
	void setPos(glm::vec3 pos);

private:
	std::vector<glm::vec3> getChunksFront();
	std::vector<glm::vec3> getChunksAround();

	std::vector<glm::vec3> getChunksTriangleNorth();
	std::vector<glm::vec3> getChunksTriangleSouth();
	std::vector<glm::vec3> getChunksTriangleEast();
	std::vector<glm::vec3> getChunksTriangleWest();

private:
	const int RENDER_DISTANCE = 14;

	glm::vec3 &_camPos;
	std::unique_ptr<Shader> _shader;
	std::unique_ptr<ChunkFactory> _factory;
	std::unique_ptr<ChunkRenderer> _renderer;
	blocking_queue<std::shared_ptr<Chunk>> _chunks;
	Camera &_camera;

	std::queue<glm::vec3> _chunksToGenerate;
	std::thread _workerThread;
	std::atomic_bool _isWorking;
	std::atomic_bool _shouldJoin;

	thread_pool _pool;
};
