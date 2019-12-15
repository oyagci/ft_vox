#include "ChunkBuilder.hpp"

ChunkBuilder::ChunkBuilder(std::shared_ptr<Chunk> chunk) :
	_hasChanged(false), _chunk(chunk), _isUpdating(true), _z(0)
{
}

void ChunkBuilder::update()
{
	if (_isUpdating) {
		updateChunk();
	}

	if (_hasChanged) {
		buildChunkMesh();
		_hasChanged = false;
	}
}

void ChunkBuilder::render()
{
	update();
	_mesh.draw();
}

int ChunkBuilder::getVisibleFaces(int x, int y, int z)
{
	int result = 0;

	bool top    = (_chunk->getBlock(x, y + 1, z) == 0);
	bool bottom = (_chunk->getBlock(x, y - 1, z) == 0);
	bool left   = (_chunk->getBlock(x - 1, y, z) == 0);
	bool right  = (_chunk->getBlock(x + 1, y, z) == 0);
	bool front  = (_chunk->getBlock(x, y, z + 1) == 0);
	bool back   = (_chunk->getBlock(x, y, z - 1) == 0);

	result |= top    ? 0 : 1 << 0;
	result |= bottom ? 0 : 1 << 1;
	result |= left   ? 0 : 1 << 2;
	result |= right  ? 0 : 1 << 3;
	result |= front  ? 0 : 1 << 4;
	result |= back   ? 0 : 1 << 5;

	return result;
}

void ChunkBuilder::updateChunk()
{
	if (!_isUpdating) {
		_isUpdating = true;
		_z = 0;
	}

	std::size_t z = _z;

	for (std::size_t x = 0; x < Chunk::CHUNK_SIZE; x++) {
		for (std::size_t y = 0; y < Chunk::CHUNK_SIZE; y++) {

			Chunk::Block b = _chunk->getBlock(x, y, z);

			if (b != 0) { continue; }

			int faces = getVisibleFaces(x, y, z);

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
	_z++;
	if (_z == Chunk::CHUNK_SIZE) {
		_isUpdating = false;
		_hasChanged = true;
	}
}

void ChunkBuilder::buildTopFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkBuilder::buildBotFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkBuilder::buildFrontFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkBuilder::buildBackFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkBuilder::buildRightFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkBuilder::buildLeftFace(Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkBuilder::buildChunkMesh()
{
	Mesh mesh;
	std::size_t nTris = 0;
	glm::vec3 worldPos = glm::vec3(_chunk->getPos().x, 0, _chunk->getPos().y);

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
	_mesh = std::move(mesh);
}

void ChunkBuilder::addFaceToRender(glm::vec3 pos, FaceDirection f)
{
	Face face = { pos, f };

	_faces.push_back(std::move(face));
}
