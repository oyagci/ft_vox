#pragma once

#include <memory>
#include <queue>
#include <map>
#include <glm/vec3.hpp>
#include "Chunk.hpp"
#include "IRenderer.hpp"
#include "threadpool/threadpool.hpp"
#include "TextRenderer.hpp"
#include "Texture.hpp"

namespace std {
	template <>
	struct hash<glm::vec2>
	{
		std::size_t operator()(const glm::vec2 &v) const
		{
			return (hash<float>()(v.x)
					^ ((hash<float>()(v.y) << 1) >> 1));
		}
	};
}

class World;

class ChunkRenderer
{
public:
	ChunkRenderer(World *world);
	~ChunkRenderer();

	void render(Camera &camera);
	void addChunk(std::shared_ptr<Chunk> chunk);
	void update();
	void removeChunksTooFar(std::vector<glm::vec2> chunksTooFar);

private:
	void buildChunks();
	bool isInView(Camera &camera, Chunk &chunk);

private:
	using Faces = std::vector<Chunk::Face>;

	std::mutex _cf;
	std::queue<std::tuple<glm::vec2, Faces>> _chunkFaces;

	thread_pool _pool;

	std::mutex _cm;
	std::queue<std::shared_ptr<Chunk>> _chunkMeshes;

	TextRenderer tr;

	World *_world;

	std::unordered_map<glm::vec2, std::shared_ptr<Chunk>> _chunkMap;

};
