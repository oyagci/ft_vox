#pragma once

#include <memory>
#include <queue>
#include <map>
#include <unordered_map>
#include <glm/vec3.hpp>
#include "ChunkController.hpp"
#include "IRenderer.hpp"
#include "TextRenderer.hpp"
#include "Texture.hpp"
#include "Hash.hpp"

class World;

class ChunkRenderer
{
public:
	ChunkRenderer(World *world);
	~ChunkRenderer();

	void render(Camera &camera, Shader &shader);
	void addChunk(std::shared_ptr<ChunkController> chunk);
	void update();
	void removeChunksTooFar(std::vector<glm::vec2> chunksTooFar);

private:
	void buildChunks();
	bool isInView(Camera &camera, ChunkController &chunk);

private:
	TextRenderer tr;

	World *_world;

	std::unordered_map<glm::vec2, std::shared_ptr<ChunkController>> _chunkMap;

};
