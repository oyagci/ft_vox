#include "WorldRenderer.hpp"

WorldRenderer::WorldRenderer(Camera &camera, glm::vec3 &camPos) : _camPos(camPos),
	_chunks(50), _camera(camera)
{
	_factory = std::make_unique<ChunkFactory>();
	_renderer = std::make_unique<ChunkRenderer>();

	_shader = std::make_unique<Shader>();
	_shader->addVertexShader("shaders/basic.vs.glsl")
		.addFragmentShader("shaders/basic.fs.glsl");
	_shader->link();

	std::vector<glm::vec3> chunks = getChunksAround();
	for (auto &c : chunks) {
		_chunksToGenerate.push(std::move(c));
	}

	chunks = getChunksTriangleNorth();
	for (auto &c : chunks) {
		_chunksToGenerate.push(std::move(c));
	}

	chunks = getChunksTriangleSouth();
	for (auto &c : chunks) {
		_chunksToGenerate.push(std::move(c));
	}

	chunks = getChunksTriangleEast();
	for (auto &c : chunks) {
		_chunksToGenerate.push(std::move(c));
	}

	chunks = getChunksTriangleWest();
	for (auto &c : chunks) {
		_chunksToGenerate.push(std::move(c));
	}

	_isWorking.store(false);
	_shouldJoin.store(false);
}

WorldRenderer::~WorldRenderer()
{
	if (_workerThread.joinable()) {
		_workerThread.join();
	}
}

void WorldRenderer::update()
{
	generateChunks();
	_renderer->update();
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
		glm::vec3 chunkPos = glm::vec3(_chunksToGenerate.front());
		_chunksToGenerate.pop();
		_pool.enqueue_work([=] {
			std::shared_ptr<Chunk> chunk = _factory->getChunk(std::move(chunkPos));
			_chunks.push(chunk);
		});
	}

	if (!_chunks.empty()) {
		auto chunk = _chunks.pop();
		_renderer->addChunk(std::move(chunk));
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

std::vector<glm::vec3> WorldRenderer::getChunksAround()
{
	std::vector<glm::vec3> chunks;
	glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

	std::array<glm::i32vec3, 9> positions = {
		glm::i32vec3(gridPos.x,     gridPos.y, gridPos.z    ), // Center
		glm::i32vec3(gridPos.x - 1, gridPos.y, gridPos.z    ), // Left
		glm::i32vec3(gridPos.x + 1, gridPos.y, gridPos.z    ), // Right
		glm::i32vec3(gridPos.x,     gridPos.y, gridPos.z + 1), // Front
		glm::i32vec3(gridPos.x,     gridPos.y, gridPos.z - 1), // Back
		glm::i32vec3(gridPos.x - 1, gridPos.y, gridPos.z + 1), // Front Left
		glm::i32vec3(gridPos.x + 1, gridPos.y, gridPos.z + 1), // Front Right
		glm::i32vec3(gridPos.x - 1, gridPos.y, gridPos.z - 1), // Back Left
		glm::i32vec3(gridPos.x + 1, gridPos.y, gridPos.z - 1), // Back Right
	};

	for (auto &p : positions) {
		glm::vec3 finalPos = glm::vec3(p) * Chunk::CHUNK_SIZE;
		chunks.push_back(glm::vec3(finalPos));
	}

	return chunks;
}

std::vector<glm::vec3> WorldRenderer::getChunksTriangleNorth()
{
	std::vector<glm::vec3> chunks;

	for (int xoff = 0; xoff < RENDER_DISTANCE; xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.z += xoff;
		for (int yoff = xoff; yoff > -xoff - 1; yoff--) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x + yoff, gridPos.y, gridPos.z) * 64.0f;
			chunks.push_back(std::move(chunkPos));
		}
	}
	return chunks;
}

std::vector<glm::vec3> WorldRenderer::getChunksTriangleSouth()
{
	std::vector<glm::vec3> chunks;

	for (int xoff = 0; xoff < RENDER_DISTANCE; xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.z -= xoff;
		for (int yoff = xoff; yoff > -xoff - 1; yoff--) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x + yoff, gridPos.y, gridPos.z) * 64.0f;
			chunks.push_back(std::move(chunkPos));
		}
	}
	return chunks;
}

std::vector<glm::vec3> WorldRenderer::getChunksTriangleEast()
{
	std::vector<glm::vec3> chunks;

	for (int xoff = 0; xoff < RENDER_DISTANCE; xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.x += xoff;
		for (int yoff = xoff; yoff > -xoff - 1; yoff--) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x, gridPos.y, gridPos.z + yoff) * 64.0f;
			chunks.push_back(std::move(chunkPos));
		}
	}
	return chunks;
}

std::vector<glm::vec3> WorldRenderer::getChunksTriangleWest()
{
	std::vector<glm::vec3> chunks;

	for (int xoff = 0; xoff < RENDER_DISTANCE; xoff++) {
		glm::i32vec3 gridPos = _camPos / Chunk::CHUNK_SIZE;

		gridPos.x -= xoff;
		for (int yoff = xoff; yoff > -xoff - 1; yoff--) {
			glm::vec3 chunkPos = glm::vec3(gridPos.x, gridPos.y, gridPos.z + yoff) * 64.0f;
			chunks.push_back(std::move(chunkPos));
		}
	}
	return chunks;
}
