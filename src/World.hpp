#pragma once
#include <memory>
#include "WorldRenderer.hpp"
#include "WorldGenerator.hpp"
#include "IRenderer.hpp"
#include <list>
#include <vector>

class World : public IRenderer
{
public:
	World(Camera &cam, glm::vec3 &camPos);

	void render() override;
	void update(glm::vec3 camPos);

private:
	std::vector<glm::vec2> getChunksTooFar(glm::vec3 camPos);
	void removeChunksTooFar(std::vector<glm::vec2> chunksTooFar);

private:
	std::unique_ptr<WorldRenderer> _renderer;
	std::unique_ptr<WorldGenerator> _builder;

	std::list<std::shared_ptr<Chunk>> _chunks;

	glm::vec3 &_camera;
};
