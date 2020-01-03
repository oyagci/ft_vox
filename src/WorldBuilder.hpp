#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <queue>
#include <mutex>
#include "ChunkFactory.hpp"
#include "threadpool/threadpool.hpp"
#include <list>

class WorldBuilder
{
public:
	WorldBuilder();

	void update();
	void setCameraPosition(glm::vec3 pos);
	std::list<std::shared_ptr<Chunk>> takeChunks();

private:
	std::vector<glm::vec3> getChunksFront();
	std::vector<glm::vec3> getChunksAround();
	std::vector<glm::vec3> getChunksTriangleNorth();
	std::vector<glm::vec3> getChunksTriangleSouth();
	std::vector<glm::vec3> getChunksTriangleEast();
	std::vector<glm::vec3> getChunksTriangleWest();

private:
	const int RENDER_DISTANCE = 6;

	std::unique_ptr<ChunkFactory> _factory;
	std::queue<glm::vec3> _chunksToGenerate;

	glm::vec3 _camPos;

	std::mutex _cl;
	std::queue<std::shared_ptr<Chunk>> _chunks;

	thread_pool _pool;
};