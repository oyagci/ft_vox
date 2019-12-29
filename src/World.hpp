#pragma once
#include <memory>
#include "WorldRenderer.hpp"
#include "WorldBuilder.hpp"
#include "IRenderer.hpp"
#include <list>

class World : public IRenderer
{
public:
	World(Camera &cam, glm::vec3 &camPos);

	void render() override;
	void update();

private:
	std::unique_ptr<WorldRenderer> _renderer;
	std::unique_ptr<WorldBuilder> _builder;

	std::list<std::shared_ptr<Chunk>> _chunks;
};
