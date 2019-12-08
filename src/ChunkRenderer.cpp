#include "ChunkRenderer.hpp"
#include <glm/vec3.hpp>

std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
	os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
	return os;
}

ChunkRenderer::ChunkRenderer()
{
}

void ChunkRenderer::registerChunk(std::shared_ptr<Chunk> chunk)
{
	ChunkInfo ci(chunk);

	ci.setChanged(true);
	_chunks.push_back(std::move(ci));
}

void ChunkRenderer::onRender()
{
	for (auto &m : _meshes) {
		m.draw();
	}
}

int ChunkRenderer::getVisibleFaces(Chunk &chunk, int x, int y, int z)
{
	int result = 0;

	bool top    = (chunk.getBlock(x, y + 1, z) == 0);
	bool bottom = (chunk.getBlock(x, y - 1, z) == 0);
	bool left   = (chunk.getBlock(x - 1, y, z) == 0);
	bool right  = (chunk.getBlock(x + 1, y, z) == 0);
	bool front  = (chunk.getBlock(x, y, z + 1) == 0);
	bool back   = (chunk.getBlock(x, y, z - 1) == 0);

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
	for (auto &c : _chunks) {
		if (c.hasChanged()) {
			updateChunk(*c.getChunk());
			buildChunkMesh(*c.getChunk());
			c.setChanged(false);
		}
	}
}

void ChunkRenderer::updateChunk(Chunk &chunk)
{
	for (std::size_t x = 0; x < Chunk::CHUNK_SIZE; x++) {
		for (std::size_t y = 0; y < Chunk::CHUNK_SIZE; y++) {
			for (std::size_t z = 0; z < Chunk::CHUNK_SIZE; z++) {

				Chunk::Block b = chunk.getBlock(x, y, z);

				if (b != 0) { continue; }

				int faces = getVisibleFaces(chunk, x, y, z);

				if (faces & (1 << 0)) { // Top
					addFaceToRender(std::move(glm::u32vec3(x, y + 1, z)), FD_BOT);
				}
				if (faces & (1 << 1)) { // Bottom
					addFaceToRender(std::move(glm::u32vec3(x, y - 1, z)), FD_TOP);
				}
				if (faces & (1 << 2)) { // Left
					addFaceToRender(std::move(glm::u32vec3(x - 1, y, z)), FD_RIGHT);
				}
				if (faces & (1 << 3)) { // Right
					addFaceToRender(std::move(glm::u32vec3(x + 1, y, z)), FD_LEFT);
				}
				if (faces & (1 << 4)) { // Front
					addFaceToRender(std::move(glm::u32vec3(x, y, z + 1)), FD_BACK);
				}
				if (faces & (1 << 5)) { // Back
					addFaceToRender(std::move(glm::u32vec3(x, y, z - 1)), FD_FRONT);
				}
			}
		}
	}
}

void ChunkRenderer::addFaceToRender(glm::vec3 pos, FaceDirection f)
{
	Face face = {
		pos,
		f
	};

	_faces.push_back(std::move(face));
}

void ChunkRenderer::buildTopFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	// Offset of the face inside the cube
	pos.y += 0.5;

	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 1, 0) + pos, glm::vec3(0, 1, 0) + pos,
		glm::vec3(1, 1, -1) + pos, glm::vec3(0, 1, -1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(1 + indOffset, 0 + indOffset, 2 + indOffset),
		glm::u32vec3(1 + indOffset, 2 + indOffset, 3 + indOffset)
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
}

void ChunkRenderer::buildBotFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	// Offset of the face inside the cube
	pos.y += 0.5;

	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 0, 0) + pos, glm::vec3(0, 0, 0) + pos,
		glm::vec3(1, 0, -1) + pos, glm::vec3(0, 0, -1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(1 + indOffset, 2 + indOffset, 0 + indOffset),
		glm::u32vec3(1 + indOffset, 3 + indOffset, 2 + indOffset)
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
}

void ChunkRenderer::buildFrontFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	// Offset of the face inside the cube
	pos.y += 0.5;

	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, 0) + pos, glm::vec3(1, 0, 0) + pos,
		glm::vec3(1, 1, 0) + pos, glm::vec3(0, 1, 0) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 1 + indOffset, 2 + indOffset),
		glm::u32vec3(0 + indOffset, 2 + indOffset, 3 + indOffset)
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
}

void ChunkRenderer::buildBackFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	// Offset of the face inside the cube
	pos.y += 0.5;

	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, -1) + pos, glm::vec3(1, 0, -1) + pos,
		glm::vec3(1, 1, -1) + pos, glm::vec3(0, 1, -1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 2 + indOffset, 1 + indOffset),
		glm::u32vec3(0 + indOffset, 3 + indOffset, 2 + indOffset)
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
}

void ChunkRenderer::buildRightFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	// Offset of the face inside the cube
	pos.y += 0.5;

	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 0, 0) + pos, glm::vec3(1, 0, -1) + pos,
		glm::vec3(1, 1, -1) + pos, glm::vec3(1, 1, 0) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 1 + indOffset, 2 + indOffset),
		glm::u32vec3(0 + indOffset, 2 + indOffset, 3 + indOffset)
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
}

void ChunkRenderer::buildLeftFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	// Offset of the face inside the cube
	pos.y += 0.5;

	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, 0) + pos, glm::vec3(0, 0, -1) + pos,
		glm::vec3(0, 1, -1) + pos, glm::vec3(0, 1, 0) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 2 + indOffset, 1 + indOffset),
		glm::u32vec3(0 + indOffset, 3 + indOffset, 2 + indOffset)
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
}

void ChunkRenderer::buildChunkMesh(Chunk &chunk)
{
	Mesh mesh;
	std::size_t nTris = 0;
	glm::vec3 worldPos = glm::vec3(chunk.getPos().x, 0, chunk.getPos().y);

	for (auto &f : _faces) {
		switch (f.dir) {
		case FD_TOP:
			buildTopFace(mesh, f.pos + worldPos, nTris);
			nTris += 4;
			break ;
		case FD_BOT:
			buildBotFace(mesh, f.pos + worldPos, nTris);
			nTris += 4;
			break ;
		case FD_FRONT:
			buildFrontFace(mesh, f.pos + worldPos, nTris);
			nTris += 4;
			break ;
		case FD_BACK:
			buildBackFace(mesh, f.pos + worldPos, nTris);
			nTris += 4;
			break ;
		case FD_RIGHT:
			buildRightFace(mesh, f.pos + worldPos, nTris);
			nTris += 4;
			break ;
		case FD_LEFT:
			buildLeftFace(mesh, f.pos + worldPos, nTris);
			nTris += 4;
		default:
			break;
		}
	}
	_faces.clear();

	mesh.build();
	_meshes.push_back(std::move(mesh));
}

std::shared_ptr<ChunkInfo> ChunkRenderer::getChunk(glm::i32vec2 pos)
{
	for (auto &c : _chunks) {
		if (static_cast<unsigned int>(c.getChunk()->getPos().x) == pos.x
			&& static_cast<unsigned int>(c.getChunk()->getPos().y) == pos.y) {
			return c;
		}
	}
	return nullptr;
}
