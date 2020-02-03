#pragma once
#include <memory>
#include "WorldRenderer.hpp"
#include "WorldGenerator.hpp"
#include "IRenderer.hpp"
#include <list>
#include <vector>
#include <lazy.hpp>

using lazy::graphics::Camera;

class World : public IRenderer
{
public:
	World(Camera &cam);

	void render() override;
	void update();

private:
	std::vector<glm::vec2> getChunksTooFar(glm::vec3 camPos);
	void removeChunksTooFar(std::vector<glm::vec2> chunksTooFar);

private:
	std::unique_ptr<WorldRenderer> _renderer;
	std::unique_ptr<WorldGenerator> _generator;

	std::list<std::shared_ptr<Chunk>> _chunks;

	Camera &_camera;
};
