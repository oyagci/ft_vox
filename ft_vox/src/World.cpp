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
	_shader->bind();
	_shader->setUniform1f("fogStart", 128.0f);
	_shader->unbind();

	_cubemap = std::make_unique<Cubemap>();
	_cubemapShader = std::make_unique<Shader>();
	_cubemapShader->addVertexShader("shaders/cubemap.vs.glsl")
		.addFragmentShader("shaders/cubemap.fs.glsl")
		.link();
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

	glm::mat4 cubemapView = glm::mat4(glm::mat3(_camera.getViewMatrix()));
	_cubemapShader->bind();
	_cubemapShader->setUniform4x4f("projectionMatrix", _camera.getProjectionMatrix());
	_cubemapShader->setUniform4x4f("viewMatrix", cubemapView);
	_cubemapShader->setUniform1i("cubemap", 0);
	_cubemap->draw();
	_cubemapShader->unbind();
}

void World::update()
{
	auto chunksTooFar = getChunksTooFar(_camera.getPosition());

	_generator->removeChunksTooFar(chunksTooFar);
	_chunkRenderer->removeChunksTooFar(chunksTooFar);
	removeChunksTooFar(chunksTooFar);

	_generator->update(_camera);
	_chunkRenderer->update();

	{
		std::unique_lock l(_chunksLock);
		auto chunks = _generator->takeChunks();
		if (!chunks.empty()) {
			// New chunks have been created
			// Add them to list of chunks
			_chunks.insert(_chunks.end(), chunks.begin(), chunks.end());

			for (auto &c : chunks) {
				_chunksRegion.set(c->getPosition(), c);
				_chunkRenderer->addChunk(c);
			}
		}
	}
}

std::vector<glm::vec2> World::getChunksTooFar(glm::vec3 camPos)
{
	glm::vec2 camPos2D(camPos.x, camPos.z);
	int renderDistance = std::any_cast<int>(Settings::instance().get("renderDistance"));
	std::vector<glm::vec2> positions;

	auto tooFar = _chunksRegion.queryOutside({ camPos2D, renderDistance * 32.0f });

	for (auto &tf : tooFar) {
		auto const &pos = tf->getPosition();
		positions.push_back(pos);
		_chunksRegion.erase(pos);
	}

	return positions;
}

void World::removeChunksTooFar(std::vector<glm::vec2> chunksTooFar)
{
	std::list<std::shared_ptr<ChunkController>> chunks;
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

	{
		std::unique_lock<std::mutex> l(_chunksLock);

		auto chunkMatches = _chunksRegion.query(glm::vec2(x, z));
		if (chunkMatches.size() > 0)
		{
			auto chunk = chunkMatches[0];

			auto &pos = chunk->getPosition();

			if (pos.x <= x && pos.x + Chunk::CHUNK_SIZE > x &&
				pos.y <= z && pos.y + Chunk::CHUNK_SIZE > z) {

				glm::uvec3 offset(x % Chunk::CHUNK_SIZE, y, z % Chunk::CHUNK_SIZE);
				auto block = chunk->getBlock(offset.x, offset.y, offset.z);
				if (block != 0) {
					return block;
				}
			}
		}
	}

	return std::nullopt;
}

std::optional<std::shared_ptr<ChunkController>> World::getChunk(glm::ivec2 pos)
{
	std::optional<std::shared_ptr<ChunkController>> chunk;

	{
		std::unique_lock<std::mutex> l(_chunksLock);
		auto chunkMatches = _chunksRegion.query(pos * Chunk::CHUNK_SIZE);

		if (chunkMatches.size() > 0) {
			return chunkMatches[0];
		}
	}

	return std::nullopt;
}

void World::setBlock(glm::ivec3 const &position, Chunk::Block value)
{
	glm::ivec2 gridPos(position.x / Chunk::CHUNK_SIZE, position.z / Chunk::CHUNK_SIZE);

	auto chunk = getChunk(gridPos);
	if (chunk) {
		chunk.value()->changeBlock({ position.x % Chunk::CHUNK_SIZE,
									  position.y % Chunk::CHUNK_SIZE,
									  position.z % Chunk::CHUNK_SIZE }, value);
	}
}
