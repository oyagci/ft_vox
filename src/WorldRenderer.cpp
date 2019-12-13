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

	std::vector<glm::vec3> chunks = getChunksFront();
	_chunksToGenerate.insert(_chunksToGenerate.end(), chunks.begin(), chunks.end());

	_isWorking.store(false);
	_shouldJoin.store(false);
}

WorldRenderer::~WorldRenderer()
{
	if (_workerThread.joinable()) {
		_workerThread.join();
	}
}

void WorldRenderer::render()
{
	generateChunks();
	_shader->bind();
	_shader->setUniform4x4f("projectionMatrix", _camera.getProjectionMatrix());
	_shader->setUniform4x4f("viewMatrix", _camera.getViewMatrix());
	_shader->setUniform4x4f("viewProjectionMatrix", _camera.getViewProjectionMatrix());
	_shader->setUniform4x4f("modelMatrix", glm::mat4(1.0f));

	_renderer->render();

	_shader->unbind();
}

void WorldRenderer::generateChunks()
{
	if (!_chunksToGenerate.empty()) {

		glm::vec3 chunkPos = glm::vec3(_chunksToGenerate.back());
		_chunksToGenerate.pop_back();
		_pool.enqueue_work([=] {
			std::shared_ptr<Chunk> chunk = _factory->getChunk(std::move(chunkPos));
			_chunks.push_back(chunk);
		});
	}

	if (!_chunks.empty()) {
		_renderer->addChunk(_chunks.back());
		_chunks.pop_back();
	}
}

void WorldRenderer::setPos(glm::vec3 pos)
{
	_camPos = std::move(pos);
}

std::vector<glm::vec3> WorldRenderer::getChunksFront()
{
	glm::i32vec3 dir = { 0, 0, -1 };
	std::vector<glm::vec3> chunks;

	for (int j = -3; j < 3; j++) {
		for (int i = 14; i >= 0; i--) {
			glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

			gridPos += (dir * i);
			gridPos *= Chunk::CHUNK_SIZE;
			gridPos += j * 64;

			chunks.push_back(glm::vec3(gridPos));
		}
	}

	return chunks;
}
