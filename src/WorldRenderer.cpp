#include "WorldRenderer.hpp"

WorldRenderer::WorldRenderer(Camera &camera, glm::vec3 &camPos) : _camPos(camPos),
	_camera(camera)
{
	_factory = std::make_unique<ChunkFactory>();
	_renderer = std::make_unique<ChunkRenderer>();

	_shader = std::make_unique<Shader>();
	_shader->addVertexShader("shaders/basic.vs.glsl")
		.addFragmentShader("shaders/basic.fs.glsl");
	_shader->link();
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& v);

void WorldRenderer::renderChunks()
{
	_shader->bind();
	_shader->setUniform4x4f("projectionMatrix", _camera.getProjectionMatrix());
	_shader->setUniform4x4f("viewMatrix", _camera.getViewMatrix());
	_shader->setUniform4x4f("viewProjectionMatrix", _camera.getViewProjectionMatrix());
	_shader->setUniform4x4f("modelMatrix", glm::mat4(1.0f));
	_renderer->onRender();
	_shader->unbind();
}

void WorldRenderer::generateChunks()
{
	const int RENDER_DISTANCE = 3;
	glm::i32vec3 gridPos(_camPos / 64);

	for (int j = -RENDER_DISTANCE; j < RENDER_DISTANCE; j++) {
		for (int i = -RENDER_DISTANCE; i < RENDER_DISTANCE; i++) {
			std::unique_ptr<Chunk> c = _factory->getChunk(
				std::move(
					glm::vec3(
						(gridPos.x + j) * 64,
						gridPos.y * 64,
						(gridPos.z + i) * 64
					)
				)
			);
			std::shared_ptr<Chunk> chunk(std::move(c));
			_chunks.push_back(chunk);
			_renderer->registerChunk(chunk);
		}
	}

	_renderer->update();
}

void WorldRenderer::setPos(glm::vec3 pos)
{
	_camPos = std::move(pos);
}
