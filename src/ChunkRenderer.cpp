#include "ChunkRenderer.hpp"
#include "stb_image.h"
#include "Settings.hpp"

ChunkRenderer::ChunkRenderer() : _pool(1)
{
	_builder = std::make_unique<ChunkBuilder>();

	int width, height, nchan;
	stbi_set_flip_vertically_on_load(1);
	unsigned char *data = stbi_load("img/terrain.png", &width, &height, &nchan, 0);

	GLuint texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
	_texture = texture;
}

ChunkRenderer::~ChunkRenderer()
{
	glDeleteTextures(1, &_texture);
}

void ChunkRenderer::addChunk(std::shared_ptr<Chunk> chunk)
{
	_chunks.push_back(chunk);
}

bool ChunkRenderer::isInView(Camera &camera, ChunkMesh &mesh)
{
	int rd = std::any_cast<int>(Settings::instance().get("renderDistance"));
	glm::vec3 pos = mesh.getPosition();
	pos.x += Chunk::CHUNK_SIZE / 2;
	pos.z += Chunk::CHUNK_SIZE / 2;

	if (glm::length(camera.getPosition() - pos) > 64.0f * (rd / 2.0f)) {
		return false;
	}

	auto vp = camera.getViewProjectionMatrix();
	glm::vec4 clipPos = vp * glm::vec4(pos, 1.0f);

	float radius = glm::length(glm::vec2(Chunk::CHUNK_SIZE / 2, Chunk::CHUNK_SIZE / 2));

	if ((-clipPos.w <= pos.x + radius && pos.x - radius <= clipPos.w) ||
		(-clipPos.w <= pos.y + radius && pos.y - radius <= clipPos.w) ||
		(-clipPos.w <= pos.z + radius && pos.z - radius <= clipPos.w)) {
		return true;
	}
	return false;
}
void ChunkRenderer::render(Camera &camera)
{
	int n = 0;
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	for (auto &m : _meshes) {
		if (isInView(camera, m)) {
			m.draw();
			n++;
		}
	}
	tr.drawText(std::to_string(n) +
		" Visible Chunks (" +
		std::to_string(_meshes.size()) + " total)",
		glm::vec2(10.0f, 30.0f), .3f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void ChunkRenderer::update()
{
	for (auto &c : _chunks) {
		if (c->shouldBeRebuilt()) {
			if (!_pool.isFull()) {
				c->setShouldBeRebuilt(false);
				_pool.enqueue_work([=] {
					ChunkBuilder builder;
					builder.setChunk(c);

					std::vector<ChunkBuilder::Face> faces = builder.genChunkFaces();
					ChunkMesh mesh = builder.build(glm::vec2(c->getPos()), faces);

					std::unique_lock<std::mutex> lcm(_cm);
					_chunkMeshes.push(ChunkMesh(std::move(mesh)));
				});
			}
		}
	}
	buildChunks();
}

void ChunkRenderer::buildChunks()
{
	std::unique_lock lm(_cm);
	if (!_chunkMeshes.empty()) {
		ChunkMesh cm = std::move(_chunkMeshes.front());
		cm.build();
		_meshes.push_back(std::move(cm));
		_chunkMeshes.pop();
	}
}
