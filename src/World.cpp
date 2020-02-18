#include "World.hpp"
#include "Settings.hpp"
#include "TextureManager.hpp"

World::World(Camera &camera) : _camera(camera)
{
	TextureManager::instance().createTexture("Blocks", "img/terrain_512.png", {
		{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
		{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },
		{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE },
	});

	_generator = std::make_unique<WorldGenerator>(this);

	_chunkRenderer = std::make_unique<ChunkRenderer>(this);

	_shader = std::make_unique<Shader>();
	_shader->addVertexShader("shaders/basic.vs.glsl")
		.addFragmentShader("shaders/basic.fs.glsl");
	_shader->link();
}

void World::render()
{
	_shader->bind();
	_shader->setUniform4x4f("projectionMatrix", _camera.getProjectionMatrix());
	_shader->setUniform4x4f("viewMatrix", _camera.getViewMatrix());
	_shader->setUniform4x4f("viewProjectionMatrix", _camera.getViewProjectionMatrix());
	_shader->setUniform4x4f("modelMatrix", glm::mat4(1.0f));

	_chunkRenderer->render(_camera, *_shader);

	_shader->unbind();
}

void World::update()
{
	auto chunksTooFar = getChunksTooFar(_camera.getPosition());

	_generator->removeChunksTooFar(chunksTooFar);
	_chunkRenderer->removeChunksTooFar(chunksTooFar);
	removeChunksTooFar(chunksTooFar);

	_generator->update(_camera);
	_chunkRenderer->update();

	auto chunks = _generator->takeChunks();
	if (!chunks.empty()) {
		// New chunks have been created
		// Add them to list of chunks
		_chunks.insert(_chunks.end(), chunks.begin(), chunks.end());

		for (auto &c : chunks) {
			_chunkRenderer->addChunk(c);
		}
	}
}

std::vector<glm::vec2> World::getChunksTooFar(glm::vec3 camPos)
{
	glm::vec2 camPos2D(camPos.x, camPos.z);
	int renderDistance = std::any_cast<int>(Settings::instance().get("renderDistance"));
	std::vector<glm::vec2> positions;

	for (auto &c : _chunks) {
		float dist = glm::length(c->getPosition() - camPos2D);

		if (dist / 64 > renderDistance) {
			positions.push_back(c->getPosition());
		}
	}

	return positions;
}

void World::removeChunksTooFar(std::vector<glm::vec2> chunksTooFar)
{
	std::list<std::shared_ptr<Chunk>> chunks;
	int n = 0;

	for (auto &c : _chunks) {
		bool remove = false;
		for (auto const &pos : chunksTooFar) {
			if (c->getPosition() == pos) {
				remove = true;
				n++;
			}
		}
		if (!remove) {
			chunks.push_back(c);
		}
	}

	_chunks = chunks;
}

std::optional<Chunk::Block> World::getBlock(int x, int y, int z)
{
	std::optional<Chunk::Block> b;

	std::unique_lock<std::mutex> l(_chunksLock);
	for (auto const &c : _chunks) {
		glm::vec2 const &pos = c->getPosition();

		if (pos.x <= x && x < pos.x + Chunk::CHUNK_SIZE &&
			pos.y <= z && z < pos.y + Chunk::CHUNK_SIZE) {

			glm::uvec3 offset(x % Chunk::CHUNK_SIZE, y, z % Chunk::CHUNK_SIZE);
			Chunk::Block block = c->getBlock(offset.x, offset.y, offset.z);
			if (block != 0) {
				b = block;
			}
			break ; 
		}
	}
	return b;
}

std::optional<std::shared_ptr<Chunk>> World::getChunk(glm::ivec2 pos)
{
	std::optional<std::shared_ptr<Chunk>> chunk;
	std::unique_lock<std::mutex> l(_chunksLock);
	for (auto &c : _chunks) {
		glm::ivec2 gridPos(c->getPosition() / Chunk::CHUNK_SIZE);
		if (gridPos == pos) {
			chunk = c;
		}
	}

	return chunk;
}
