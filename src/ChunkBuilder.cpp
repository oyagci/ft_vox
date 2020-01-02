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

auto ChunkBuilder::genFaceToRender(glm::vec3 pos, FaceDirection f, Chunk::Block const &block) -> Face
{
	Face face = { pos, f, AIR };

	if (block == 1) {
		face.type = GRASS;
	}
	else if (block == 2) {
		face.type = STONE;
	}

	return face;
}

auto ChunkBuilder::genChunkFaces() -> std::vector<Face>
{
	std::vector<Face> faces;

	for (std::size_t x = 0; x < Chunk::CHUNK_SIZE; x++) {
		for (std::size_t y = 0; y < Chunk::CHUNK_SIZE; y++) {
			for (std::size_t z = 0; z < Chunk::CHUNK_SIZE; z++) {

				Chunk::Block b = _chunk->getBlock(x, y, z);

				if (b) {
					if (y == 0) {
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z)), FD_BOT, b));
					}
					else if (y == Chunk::CHUNK_SIZE - 1) {
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z)), FD_TOP, b));
					}

					if (x == 0) {
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z)), FD_LEFT, b));
					}
					else if (x == Chunk::CHUNK_SIZE - 1) {
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z)), FD_RIGHT, b));
					}

					if (z == Chunk::CHUNK_SIZE - 1) {
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z)), FD_FRONT, b));
					}
					else if (z == 0) {
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z)), FD_BACK, b));
					}
				}
				else {
					int visibleFaces = getVisibleFaces(x, y, z);

					if (visibleFaces & (1 << 0)) { // Top
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y + 1, z)), FD_BOT,
							_chunk->getBlock(x, y + 1, z)));
					}
					if (visibleFaces & (1 << 1)) { // Bottom
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y - 1, z)), FD_TOP,
							_chunk->getBlock(x, y - 1, z)));
					}
					if (visibleFaces & (1 << 2)) { // Left
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x - 1, y, z)), FD_RIGHT,
							_chunk->getBlock(x - 1, y, z)));
					}
					if (visibleFaces & (1 << 3)) { // Right
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x + 1, y, z)), FD_LEFT,
							_chunk->getBlock(x + 1, y, z)));
					}
					if (visibleFaces & (1 << 4)) { // Front
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z + 1)), FD_BACK,
							_chunk->getBlock(x, y, z + 1)));
					}
					if (visibleFaces & (1 << 5)) { // Back
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z - 1)), FD_FRONT,
							_chunk->getBlock(x, y, z - 1)));
					}
				}
			}
		}
	}
	return faces;
}

void ChunkBuilder::buildTopFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 1, 0) + pos, glm::vec3(0, 1, 0) + pos,
		glm::vec3(1, 1, -1) + pos, glm::vec3(0, 1, -1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(1 + indOffset, 0 + indOffset, 2 + indOffset),
		glm::u32vec3(1 + indOffset, 2 + indOffset, 3 + indOffset)
	};

	unsigned int xindex = 0;
	unsigned int yindex = 0;

	if (face.type == GRASS) {
		xindex = 8;
		yindex = 4;
	}
	else if (face.type == STONE) {
		xindex = 1;
		yindex = 15;
	}

	float side = 16.0f / 256.0f;
	float xoff = xindex * side;
	float yoff = yindex * side;

	std::array<glm::vec2, 4> texture = {
		glm::vec2(side + xoff, yoff),
		glm::vec2(       xoff, yoff),
		glm::vec2(side + xoff, side + yoff),
		glm::vec2(       xoff, side + yoff),
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
		mesh.addNormal(glm::vec3(0.0f, 1.0f, 0.0f));
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
	for (auto &t : texture) {
		mesh.addUv(t);
	}
}

void ChunkBuilder::buildBotFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 0, 0) + pos, glm::vec3(0, 0, 0) + pos,
		glm::vec3(1, 0, -1) + pos, glm::vec3(0, 0, -1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(1 + indOffset, 2 + indOffset, 0 + indOffset),
		glm::u32vec3(1 + indOffset, 3 + indOffset, 2 + indOffset)
	};

	unsigned int xindex = 0;
	unsigned int yindex = 0;

	if (face.type == GRASS) {
		xindex = 2;
		yindex = 15;
	}
	else if (face.type == STONE) {
		xindex = 1;
		yindex = 15;
	}

	float side = 16.0f / 256.0f;
	float xoff = xindex * side;
	float yoff = yindex * side;

	std::array<glm::vec2, 4> texture = {
		glm::vec2(side + xoff, yoff),
		glm::vec2(xoff, yoff),
		glm::vec2(side + xoff, side + yoff),
		glm::vec2(xoff, side + yoff),
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
		mesh.addNormal(glm::vec3(0.0f, -1.0f, 0.0f));
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
	for (auto &t : texture) {
		mesh.addUv(t);
	}
}

void ChunkBuilder::buildFrontFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, 0) + pos, glm::vec3(1, 0, 0) + pos,
		glm::vec3(1, 1, 0) + pos, glm::vec3(0, 1, 0) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 1 + indOffset, 2 + indOffset),
		glm::u32vec3(0 + indOffset, 2 + indOffset, 3 + indOffset)
	};

	unsigned int xindex = 0;
	unsigned int yindex = 0;

	if (face.type == GRASS) {
		xindex = 3;
		yindex = 15;
	}
	else if (face.type == STONE) {
		xindex = 1;
		yindex = 15;
	}

	float side = 16.0f / 256.0f;
	float xoff = xindex * side;
	float yoff = yindex * side;

	std::array<glm::vec2, 4> texture = {
		glm::vec2(xoff, yoff),
		glm::vec2(side + xoff, yoff),
		glm::vec2(side + xoff, side + yoff),
		glm::vec2(xoff, side + yoff),
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
		mesh.addNormal(glm::vec3(0.0f, 0.0f, -1.0f));
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
	for (auto &t : texture) {
		mesh.addUv(t);
	}
}

void ChunkBuilder::buildBackFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, -1) + pos, glm::vec3(1, 0, -1) + pos,
		glm::vec3(1, 1, -1) + pos, glm::vec3(0, 1, -1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 2 + indOffset, 1 + indOffset),
		glm::u32vec3(0 + indOffset, 3 + indOffset, 2 + indOffset)
	};

	unsigned int xindex = 0;
	unsigned int yindex = 0;

	if (face.type == GRASS) {
		xindex = 3;
		yindex = 15;
	}
	else if (face.type == STONE) {
		xindex = 1;
		yindex = 15;
	}

	float side = 16.0f / 256.0f;
	float xoff = xindex * side;
	float yoff = yindex * side;

	std::array<glm::vec2, 4> texture = {
		glm::vec2(xoff, yoff),
		glm::vec2(side + xoff, yoff),
		glm::vec2(side + xoff, side + yoff),
		glm::vec2(xoff, side + yoff),
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
		mesh.addNormal(glm::vec3(0.0f, 0.0f, 1.0f));
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
	for (auto &t : texture) {
		mesh.addUv(t);
	}
}

void ChunkBuilder::buildRightFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 0, 0) + pos, glm::vec3(1, 0, -1) + pos,
		glm::vec3(1, 1, -1) + pos, glm::vec3(1, 1, 0) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 1 + indOffset, 2 + indOffset),
		glm::u32vec3(0 + indOffset, 2 + indOffset, 3 + indOffset)
	};

	unsigned int xindex = 0;
	unsigned int yindex = 0;

	if (face.type == GRASS) {
		xindex = 3;
		yindex = 15;
	}
	else if (face.type == STONE) {
		xindex = 1;
		yindex = 15;
	}

	float side = 16.0f / 256.0f;
	float xoff = xindex * side;
	float yoff = yindex * side;

	std::array<glm::vec2, 4> texture = {
		glm::vec2(xoff, yoff),
		glm::vec2(side + xoff, yoff),
		glm::vec2(side + xoff, side + yoff),
		glm::vec2(xoff, side + yoff),
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
		mesh.addNormal(glm::vec3(1.0f, 0.0f, 0.0f));
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
	for (auto &t : texture) {
		mesh.addUv(t);
	}
}

void ChunkBuilder::buildLeftFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, 0) + pos, glm::vec3(0, 0, -1) + pos,
		glm::vec3(0, 1, -1) + pos, glm::vec3(0, 1, 0) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 2 + indOffset, 1 + indOffset),
		glm::u32vec3(0 + indOffset, 3 + indOffset, 2 + indOffset)
	};

	unsigned int xindex = 0;
	unsigned int yindex = 0;

	if (face.type == GRASS) {
		xindex = 3;
		yindex = 15;
	}
	else if (face.type == STONE) {
		xindex = 1;
		yindex = 15;
	}

	float side = 16.0f / 256.0f;
	float xoff = xindex * side;
	float yoff = yindex * side;

	std::array<glm::vec2, 4> texture = {
		glm::vec2(xoff, yoff),
		glm::vec2(side + xoff, yoff),
		glm::vec2(side + xoff, side + yoff),
		glm::vec2(xoff, side + yoff),
	};

	for (auto &v : vertices) {
		mesh.addPosition(v);
		mesh.addNormal(glm::vec3(-1.0f, 0.0f, 0.0f));
	}
	for (auto &t : triangles) {
		mesh.addTriangle(t);
	}
	for (auto &t : texture) {
		mesh.addUv(t);
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
			buildTopFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_BOT:
			buildBotFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_FRONT:
			buildFrontFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_BACK:
			buildBackFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_RIGHT:
			buildRightFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FD_LEFT:
			buildLeftFace(f, mesh, f.pos + worldPos, nVert);
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
