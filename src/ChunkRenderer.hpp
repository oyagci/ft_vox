#pragma once

#include <memory>
#include <queue>
#include <map>
#include <glm/vec3.hpp>
#include "Chunk.hpp"
#include "IRenderer.hpp"
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

	void render(Camera &camera, Shader &shader);
	void addChunk(std::shared_ptr<Chunk> chunk);
	void update();
	void removeChunksTooFar(std::vector<glm::vec2> chunksTooFar);

private:
	void buildChunks();
	bool isInView(Camera &camera, Chunk &chunk);

private:
	TextRenderer tr;

	World *_world;

	std::unordered_map<glm::vec2, std::shared_ptr<Chunk>> _chunkMap;

};
