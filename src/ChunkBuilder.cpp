#include "ChunkBuilder.hpp"

ChunkBuilder::ChunkBuilder()
{
}

Mesh ChunkBuilder::build(glm::vec2 pos, std::vector<Face> faces)
{
	Mesh mesh = buildChunkFaces(std::move(pos), std::move(faces));

	return mesh;
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

auto ChunkBuilder::genFaceToRender(glm::vec3 pos, FaceDirection f) -> Face
{
	Face face = { pos, f };

	return face;
}

auto ChunkBuilder::genChunkFaces() -> std::vector<Face>
{
	std::vector<Face> faces;

	for (std::size_t x = 0; x < Chunk::CHUNK_SIZE; x++) {
		for (std::size_t y = 0; y < Chunk::CHUNK_SIZE; y++) {
			for (std::size_t z = 0; z < Chunk::CHUNK_SIZE; z++) {

				Chunk::Block b = _chunk->getBlock(x, y, z);

				if (b != 0) { continue; }

				int visibleFaces = getVisibleFaces(x, y, z);

				if (visibleFaces & (1 << 0)) { // Top
					faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y + 1, z)), FD_BOT));
				}
				if (visibleFaces & (1 << 1)) { // Bottom
					faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y - 1, z)), FD_TOP));
				}
				if (visibleFaces & (1 << 2)) { // Left
					faces.push_back(genFaceToRender(std::move(glm::u32vec3(x - 1, y, z)), FD_RIGHT));
				}
				if (visibleFaces & (1 << 3)) { // Right
					faces.push_back(genFaceToRender(std::move(glm::u32vec3(x + 1, y, z)), FD_LEFT));
				}
				if (visibleFaces & (1 << 4)) { // Front
					faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z + 1)), FD_BACK));
				}
				if (visibleFaces & (1 << 5)) { // Back
					faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z - 1)), FD_FRONT));
				}
			}
		}
	}
	return faces;
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

Mesh ChunkBuilder::buildChunkFaces(glm::vec2 chunkPos, std::vector<Face> faces)
{
	Mesh mesh;
	std::size_t nVert = 0;
	glm::vec3 worldPos = glm::vec3(chunkPos.x, 0, chunkPos.y);

	for (auto &f : faces) {
		switch (f.dir) {
		case FD_TOP:
			buildTopFace(mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_BOT:
			buildBotFace(mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_FRONT:
			buildFrontFace(mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_BACK:
			buildBackFace(mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_RIGHT:
			buildRightFace(mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_LEFT:
			buildLeftFace(mesh, f.pos + worldPos, nVert);
			nVert += 4;
		default:
			break;
		}
	}

	return mesh;
}

Mesh ChunkBuilder::buildChunkMesh(glm::vec2 chunkPos, std::vector<Face> faces)
{
	Mesh mesh;

	mesh = std::move(buildChunkFaces(std::move(chunkPos), std::move(faces)));
	mesh.build();
	return mesh;
}
