#include "ChunkRenderer.hpp"
#include <glm/vec3.hpp>

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
	os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
	return os;
}

ChunkRenderer::ChunkRenderer() : _chunk(nullptr)
{
	glm::vec3 vertices[] = {
		glm::vec3(0, 0,  0), glm::vec3(1, 0,  0), glm::vec3(1, 1,  0), glm::vec3(0, 1,  0),
		glm::vec3(0, 0, -1), glm::vec3(1, 0, -1), glm::vec3(1, 1, -1), glm::vec3(0, 1, -1),
	};
	glm::vec3 triangles[] = {
		glm::vec3(0, 1, 2), glm::vec3(0, 2, 3), // Front
		glm::vec3(6, 5, 4), glm::vec3(7, 6, 4), // Back
		glm::vec3(0, 3, 7), glm::vec3(0, 7, 4), // Left
		glm::vec3(1, 5, 6), glm::vec3(1, 6, 2), // Right
		glm::vec3(3, 2, 6), glm::vec3(3, 6, 7), // Top
		glm::vec3(0, 5, 1), glm::vec3(0, 4, 5), // Bottom
	};

	for (auto &v : vertices) {
		_mesh.addPosition(std::move(v));
	}
	for (auto &t : triangles) {
		_mesh.addTriangle(std::move(t));
	}
	_mesh.build();
}

void ChunkRenderer::setChunk(Chunk chunk)
{
	_chunk = std::move(chunk);
}

void ChunkRenderer::setShader(Shader *shader)
{
	_shader = shader;
}

void ChunkRenderer::onRender()
{
	_mesh.draw();
}

int ChunkRenderer::getVisibleFaces(int x, int y, int z)
{
	int result = 0;

	bool top    = (_chunk.getBlock(x, y + 1, z) == 0);
	bool bottom = (_chunk.getBlock(x, y - 1, z) == 0);
	bool left   = (_chunk.getBlock(x - 1, y, z) == 0);
	bool right  = (_chunk.getBlock(x + 1, y, z) == 0);
	bool front  = (_chunk.getBlock(x, y, z + 1) == 0);
	bool back   = (_chunk.getBlock(x, y, z - 1) == 0);

	result |= top    ? 0 : 1 << 0;
	result |= bottom ? 0 : 1 << 1;
	result |= left   ? 0 : 1 << 2;
	result |= right  ? 0 : 1 << 3;
	result |= front  ? 0 : 1 << 4;
	result |= back   ? 0 : 1 << 5;

	return result;
}

void ChunkRenderer::update()
{
	for (std::size_t x = 0; x < Chunk::CHUNK_SIZE; x++) {
		for (std::size_t y = 0; y < Chunk::CHUNK_SIZE; y++) {
			for (std::size_t z = 0; z < Chunk::CHUNK_SIZE; z++) {

				Chunk::Block b = _chunk.getBlock(x, y, z);

				if (b != 0) { continue; }

				int faces = getVisibleFaces(x, y, z);

				if (faces & (1 << 0)) { // Top
					addBlockToRender(std::move(glm::u32vec3(x, y + 1, z)));
				}
				if (faces & (1 << 1)) { // Bottom
					addBlockToRender(std::move(glm::u32vec3(x, y - 1, z)));
				}
				if (faces & (1 << 2)) { // Left
					addBlockToRender(std::move(glm::u32vec3(x - 1, y, z)));
				}
				if (faces & (1 << 3)) { // Right
					addBlockToRender(std::move(glm::u32vec3(x + 1, y, z)));
				}
				if (faces & (1 << 4)) { // Front
					addBlockToRender(std::move(glm::u32vec3(x, y, z + 1)));
				}
				if (faces & (1 << 5)) { // Back
					addBlockToRender(std::move(glm::u32vec3(x, y, z - 1)));
				}
			}
		}
	}
	buildChunkMesh();
}

void ChunkRenderer::buildChunkMesh()
{
	Mesh chunk;

	std::vector<glm::vec3> chunkVerts;
	std::vector<glm::vec3> chunkTris;
	std::size_t nTris = 0;

	// Base cube vertices and triangles used to build the chunk's mesh
	std::array<glm::vec3, 8> vertices = {
		glm::vec3(0, 0,  0), glm::vec3(1, 0,  0), glm::vec3(1, 1,  0), glm::vec3(0, 1,  0),
		glm::vec3(0, 0, -1), glm::vec3(1, 0, -1), glm::vec3(1, 1, -1), glm::vec3(0, 1, -1),
	};
	std::array<glm::u32vec3, 12> triangles = {
		glm::u32vec3(0, 1, 2), glm::u32vec3(0, 2, 3), // Front
		glm::u32vec3(6, 5, 4), glm::u32vec3(7, 6, 4), // Back
		glm::u32vec3(0, 3, 7), glm::u32vec3(0, 7, 4), // Left
		glm::u32vec3(1, 5, 6), glm::u32vec3(1, 6, 2), // Right
		glm::u32vec3(3, 2, 6), glm::u32vec3(3, 6, 7), // Top
		glm::u32vec3(0, 5, 1), glm::u32vec3(0, 4, 5), // Bottom
	};

	// Gather all vertices and triangles into two vectors
	for (auto &b : _blocks) {
		for (auto &v : vertices) {
			glm::vec3 verts = v + b;
			chunkVerts.push_back(std::move(verts));
		}
		for (auto &t : triangles) {
			glm::u32vec3 inds(t.x + nTris, t.y + nTris, t.z + nTris);
			chunkTris.push_back(std::move(inds));
		}
		nTris += vertices.size();
	}

	// Add every vertices and triangles of the chunk to its mesh
	for (auto &v : chunkVerts) {
		chunk.addPosition(v);
	}
	for (auto &t : chunkTris) {
		chunk.addTriangle(t);
	}
	chunk.build();

	_mesh = std::move(chunk);
}

void ChunkRenderer::addBlockToRender(glm::vec3 pos)
{
	_blocks.push_back(std::move(pos));
}
