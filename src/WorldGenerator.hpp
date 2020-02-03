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
#include "TextRenderer.hpp"

class WorldRenderer;

class WorldGenerator
{
public:
	WorldGenerator(WorldRenderer *wr);

	void update(Camera const &camera);
	void setCameraPosition(glm::vec3 pos);
	std::list<std::shared_ptr<Chunk>> takeChunks();
	void removeChunksTooFar(std::vector<glm::vec2> chunksTooFar);

private:
	void genChunksAroundPlayer();

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
	void addChunkToGenerate(glm::vec3 pos);

private:
	const int RENDER_DISTANCE = 6;

	std::unique_ptr<ChunkFactory> _factory;
	std::list<glm::vec3> _chunksToGenerate;
	std::priority_queue<ChunkPriority, std::vector<ChunkPriority>, std::greater<ChunkPriority>> _priority;

	glm::vec3 _camPos;

	std::mutex _cl;
	std::queue<std::shared_ptr<Chunk>> _chunks;
	std::vector<glm::vec3> _generatedChunks;

	thread_pool _pool;

	glm::vec3 lastGridPos;
};

