#pragma once
#include <memory>
#include "WorldRenderer.hpp"
#include "IRenderer.hpp"

class World : public IRenderer
{
public:
	World(Camera &cam, glm::vec3 &camPos);

	void render() override;
	void update();

private:
	std::unique_ptr<WorldRenderer> _renderer;
};
