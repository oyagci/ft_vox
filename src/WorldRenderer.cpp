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

	glm::i32vec3 gridPos(camPos / 64);
	for (int j = -RENDER_DISTANCE; j < RENDER_DISTANCE; j++) {
		for (int i = -RENDER_DISTANCE; i < RENDER_DISTANCE; i++) {
			glm::vec3 pos = glm::vec3(
					(gridPos.x + j) * 64,
					gridPos.y * 64,
					(gridPos.z + i) * 64);
			_chunksToGenerate.push_back(std::move(pos));
		}
	}

	_isWorking.store(false);
	_shouldJoin.store(false);
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
	if (!_chunksToGenerate.empty() && !_isWorking.load()) {
		_isWorking.store(true);
		_workerThread = std::thread([&] {
			auto pos = _chunksToGenerate.back();
			std::shared_ptr<Chunk> chunk = _factory->getChunk(pos);
			_chunks.push_back(chunk);
			_renderer->registerChunk(chunk);
			_chunksToGenerate.pop_back();
			_shouldJoin.store(true);
		});
	}
	else if (_shouldJoin.load()) {
		_workerThread.join();
		_isWorking.store(false);
		_shouldJoin.store(false);
		_renderer->update();
	}
}

void WorldRenderer::setPos(glm::vec3 pos)
{
	_camPos = std::move(pos);
}
