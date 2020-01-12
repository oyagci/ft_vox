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
#include <list>
#include <mutex>

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
	void registerChunks(std::list<std::shared_ptr<Chunk>> chunks);
	Chunk::Block getBlock(int x, int y, int z);

private:
	const int RENDER_DISTANCE = 6;

	glm::vec3 &_camPos;
	std::unique_ptr<Shader> _shader;
	std::unique_ptr<ChunkRenderer> _renderer;

	std::mutex _chunksLock;
	std::vector<std::shared_ptr<Chunk>> _chunks;
	Camera &_camera;
};
