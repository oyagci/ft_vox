#include "WorldRenderer.hpp"

WorldRenderer::WorldRenderer(Camera &camera, glm::vec3 &camPos) : _camPos(camPos),
	_camera(camera)
{
	_renderer = std::make_unique<ChunkRenderer>(this);

	_shader = std::make_unique<Shader>();
	_shader->addVertexShader("shaders/basic.vs.glsl")
		.addFragmentShader("shaders/basic.fs.glsl");
	_shader->link();
}

WorldRenderer::~WorldRenderer()
{
}

void WorldRenderer::update()
{
	if (!_chunks.empty()) {
		_renderer->addChunk(_chunks.back());
		_chunks.pop_back();
	}
	_renderer->update();
}

void WorldRenderer::render()
{
	_shader->bind();
	_shader->setUniform4x4f("projectionMatrix", _camera.getProjectionMatrix());
	_shader->setUniform4x4f("viewMatrix", _camera.getViewMatrix());
	_shader->setUniform4x4f("viewProjectionMatrix", _camera.getViewProjectionMatrix());
	_shader->setUniform4x4f("modelMatrix", glm::mat4(1.0f));

	_renderer->render(_camera);

	_shader->unbind();
}

void WorldRenderer::setPos(glm::vec3 pos)
{
	_camPos = std::move(pos);
}

void WorldRenderer::registerChunks(std::list<std::shared_ptr<Chunk>> chunks)
{
	_chunks.insert(_chunks.end(), chunks.begin(), chunks.end());
}
