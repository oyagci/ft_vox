#include "WorldRenderer.hpp"
#include <iostream>

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
	std::unique_lock<std::mutex> l(_chunksLock);
	_chunks.insert(_chunks.end(), chunks.begin(), chunks.end());
	for (auto &c : _chunks) { _renderer->addChunk(std::shared_ptr<Chunk>(c)); }
}

std::ostream &operator<<(std::ostream &os, glm::vec3 v) {
	os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
	return os;
}

std::ostream &operator<<(std::ostream &os, glm::uvec3 v) {
	os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
	return os;
}

std::ostream &operator<<(std::ostream &os, glm::vec2 v) {
	os << "{ " << v.x << ", " << v.y << " }";
	return os;
}

Chunk::Block WorldRenderer::getBlock(int x, int y, int z)
{
	std::unique_lock<std::mutex> l(_chunksLock);
	for (auto const &c : _chunks) {
		glm::vec2 const &pos = c->getPos();

		if (pos.x <= x && x < pos.x + Chunk::CHUNK_SIZE &&
			pos.y <= z && z < pos.y + Chunk::CHUNK_SIZE) {

			glm::uvec3 offset(x % Chunk::CHUNK_SIZE, y, z % Chunk::CHUNK_SIZE);
			return c->getBlock(offset.x, offset.y, offset.z);
		}
	}
	return 0;
}
