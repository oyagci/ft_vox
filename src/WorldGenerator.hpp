#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <queue>
#include <mutex>
#include "ChunkFactory.hpp"
#include "threadpool/threadpool.hpp"
#include <list>
#include <queue>

class WorldGenerator
{
public:
	WorldGenerator();

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

	using Priority = unsigned int;
	struct ChunkPriority {
		Priority priority;
		glm::vec3 position;
		ChunkPriority(Priority pri, glm::vec3 pos) : priority(pri), position(pos) {}
		friend bool operator<(const ChunkPriority &lhs, const ChunkPriority &rhs) {
			return lhs.priority < rhs.priority;
		}
		friend bool operator>(const ChunkPriority &lhs, const ChunkPriority &rhs) {
			return lhs.priority > rhs.priority;
		}
	};
	void addChunkToGenerate(glm::vec3 pos, Priority priority);
	glm::vec3 popPriorityChunk();

private:
	const int RENDER_DISTANCE = 6;

	std::unique_ptr<ChunkFactory> _factory;
	std::priority_queue<ChunkPriority, std::vector<ChunkPriority>,std::greater<ChunkPriority>> _chunksToGenerate;

	glm::vec3 _camPos;

	std::mutex _cl;
	std::queue<std::shared_ptr<Chunk>> _chunks;

	thread_pool _pool;
};
