#include "Chunk.hpp"
#include <cstdint>
#include "SimplexNoise.hpp"
#include <mutex>
#include "WorldRenderer.hpp"

SimplexNoise s = SimplexNoise(0.1f, 1.0f, 2.0f, 0.25f);

float simplexNoise(size_t octaves, glm::vec3 pos)
{
	return s.fractal(octaves, pos.x, pos.y, pos.z);
}

Chunk::Chunk(glm::i32vec2 pos, WorldRenderer *wr) : _shouldBeRebuilt(true), _worldRenderer(wr)
{
	_blocks = std::make_unique<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>>();
	_worldPos = std::move(pos);

	for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
		for (std::size_t y = CHUNK_SIZE - 1; y + 1 > 0; y--) {
			for (std::size_t z = 0; z < CHUNK_SIZE; z++) {

				if (y == 0) {
					setBlock(x, y, z, 4);
					continue ;
				}

				glm::vec3 pos = glm::vec3(_worldPos.x, -32, _worldPos.y) + glm::vec3(x, y, z);
				float result = 0.0f;

				float hH = simplexNoise(1, pos / 10);
				for (int i = 2; i < 6; i++) {
					hH += simplexNoise(1, pos / (i * 5)) * i / 16;
				}

				float pH = simplexNoise(1, pos / 10);
				for (int i = 2; i < 6; i++) {
					pH -= simplexNoise(1, pos / i) * i / 16;
				}

				glm::vec3 mP = glm::vec3(pos.x + 3000.0f, 0, pos.z -400) / 50;
				float mm = simplexNoise(1, mP);

				glm::vec3 dP = glm::vec3(pos.x - 3.4f, 827, z + 51.3f);
				float path = simplexNoise(1, dP);

				for (int i = 2; i < 6; i++) {
					path *= simplexNoise(1, dP / i) * i / 6;
				}

				glm::vec3 cP = glm::vec3(pos.x / 2 - 523.432, pos.y - 2.827, z / 2 + 5516.0f);
				float cave = simplexNoise(1, cP / 5);
				for (int i = 0; i < 6; i++) {
					cave += simplexNoise(1, cP / (i * 2)) * i / 6;
				}

				if (abs(floor(cave * 50)) - 30 > 0) {
					cave = abs(cave * 10);
				}
				else {
					cave = 0;
				}

				result = -pos.y + (pH / 2 + hH) * (CHUNK_SIZE / 2) * mm + path * 10 - cave * abs(mm);

				if (getBlock(x, y + 1, z) > 0) {
					if (getBlock(x, y + 2, z) > 0) {
						if (getBlock(x, y + 3, z) > 0) {
							setBlock(x, y, z, (y == 0 || result > 0.0f ? 2 : 0));
						}
						else {
							setBlock(x, y, z, (y == 0 || result > 0.0f ? 3 : 0));
						}
					}
					else {
						setBlock(x, y, z, (y == 0 || result > 0.0f ? 3 : 0));
					}
				}
				else {
					setBlock(x, y, z, (y == 0 || result > 0.0f ? 1 : 0));
				}
			}
		}
	}
}

auto Chunk::getBlock(std::size_t x, std::size_t y, std::size_t z) const -> Block
{
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return (_void);
	}

	return (*_blocks)[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z];
}

void Chunk::setBlock(size_t x, size_t y, size_t z, Block val)
{
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
		return ;
	}
	(*_blocks)[x * CHUNK_SIZE * CHUNK_SIZE + y * CHUNK_SIZE + z] = val;
}

const glm::vec2 &Chunk::getPos() const
{
	return _worldPos;
}

void Chunk::markMissingNeighbors()
{
	glm::ivec2 gridPos(getPos() / CHUNK_SIZE);

	auto c = _worldRenderer->getChunk(glm::ivec2(gridPos.x - 1, gridPos.y));
	if (!c.has_value()) {
		_missingNeighbors |= LEFT;
	}

	c = _worldRenderer->getChunk(glm::ivec2(gridPos.x + 1, gridPos.y));
	if (!c.has_value()) {
		_missingNeighbors |= RIGHT;
	}

	c = _worldRenderer->getChunk(glm::ivec2(gridPos.x, gridPos.y - 1));
	if (!c.has_value()) {
		_missingNeighbors |= FRONT;
	}

	c = _worldRenderer->getChunk(glm::ivec2(gridPos.x - 1, gridPos.y + 1));
	if (!c.has_value()) {
		_missingNeighbors |= BACK;
	}
}

void Chunk::build()
{
	markMissingNeighbors();
	_isBuilt = _missingNeighbors == 0;
	_faces = genChunkFaces();
	_mesh = buildChunkFaces(_worldPos, _faces);
	_mesh.build();
}

void Chunk::draw()
{
	_mesh.draw();
}

int Chunk::getVisibleFaces(int x, int y, int z)
{
	int result = 0;

	glm::ivec3 worldPos(getPos().x + x, y, getPos().y + z);

	bool top;
	bool bottom;
	bool left;
	bool right;
	bool front;
	bool back;

	auto blockIsEmpty = [this] (int x, int y, int z) -> bool {
		auto b = _worldRenderer->getBlock(x, y, z);
		return !b.has_value() || b.value() == 0;
	};

	if (y + 1 >= Chunk::CHUNK_SIZE) {
		top = blockIsEmpty(worldPos.x, worldPos.y + 1, worldPos.z);
	}
	else {
		top = (getBlock(x, y + 1, z) == 0);
	}

	if (y - 1 <= 0) {
		bottom = blockIsEmpty(worldPos.x, worldPos.y - 1, worldPos.z);
	}
	else {
		bottom = (getBlock(x, y - 1, z) == 0);
	}

	if (x - 1 <= 0) {
		left = blockIsEmpty(worldPos.x - 1, worldPos.y, worldPos.z);
	}
	else {
		left = (getBlock(x - 1, y, z) == 0);
	}

	if (x + 1 >= CHUNK_SIZE) {
		right = blockIsEmpty(worldPos.x + 1, worldPos.y, worldPos.z);
	}
	else {
		right = (getBlock(x + 1, y, z) == 0);
	}

	if (z + 1 > CHUNK_SIZE) {
		front = blockIsEmpty(worldPos.x, worldPos.y, worldPos.z + 1);
	}
	else {
		front = (getBlock(x, y, z + 1) == 0);
	}

	if (z - 1 <= 0) {
		back = blockIsEmpty(worldPos.x, worldPos.y, worldPos.z - 1);
	}
	else {
		back = (getBlock(x, y, z - 1) == 0);
	}

	result |= top    ? 0 : 1 << 0;
	result |= bottom ? 0 : 1 << 1;
	result |= left   ? 0 : 1 << 2;
	result |= right  ? 0 : 1 << 3;
	result |= front  ? 0 : 1 << 4;
	result |= back   ? 0 : 1 << 5;

	return result;
}

auto Chunk::genFaceToRender(glm::vec3 pos, FaceDirection f, Chunk::Block const &block) -> Face
{
	Face face = { pos, f, AIR };

	if (block == 1 && (f == FD_TOP)) {
		face.type = GRASS;
	}
	else if (block == 1 && (f == FD_LEFT || f == FD_RIGHT || f == FD_FRONT || f == FD_BACK)) {
		face.type = GRASS_SIDE;
	}
	else if (block == 1 && (f == FD_BOT)) {
		face.type = DIRT;
	}
	else if (block == 2) {
		face.type = STONE;
	}
	else if (block == 3) {
		face.type = DIRT;
	}
	else if (block == 4) {
		face.type = BEDROCK;
	}

	return face;
}

auto Chunk::genChunkFaces() -> std::vector<Face>
{
	std::vector<Face> faces;

	for (std::size_t x = 0; x < Chunk::CHUNK_SIZE; x++) {
		for (std::size_t y = 0; y < Chunk::CHUNK_SIZE; y++) {
			for (std::size_t z = 0; z < Chunk::CHUNK_SIZE; z++) {

				Chunk::Block b = getBlock(x, y, z);

				if (!b) {
					int visibleFaces = getVisibleFaces(x, y, z);

					if (visibleFaces & (1 << 0)) { // Top
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y + 1, z)), FD_BOT,
							getBlock(x, y + 1, z)));
					}
					if (visibleFaces & (1 << 1)) { // Bottom
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y - 1, z)), FD_TOP,
							getBlock(x, y - 1, z)));
					}
					if (visibleFaces & (1 << 2)) { // Left
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x - 1, y, z)), FD_RIGHT,
							getBlock(x - 1, y, z)));
					}
					if (visibleFaces & (1 << 3)) { // Right
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x + 1, y, z)), FD_LEFT,
							getBlock(x + 1, y, z)));
					}
					if (visibleFaces & (1 << 4)) { // Front
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z + 1)), FD_BACK,
							getBlock(x, y, z + 1)));
					}
					if (visibleFaces & (1 << 5)) { // Back
						faces.push_back(genFaceToRender(std::move(glm::u32vec3(x, y, z - 1)), FD_FRONT,
							getBlock(x, y, z - 1)));
					}
				}
			}
		}
	}
	return faces;
}

glm::vec2 Chunk::getTexturePosition(BlockType type)
{
	if (type == GRASS) {
		return glm::vec2(TEXTURE_TILE_SIZE * 8, TEXTURE_TILE_SIZE * 4);
	}
	else if (type == GRASS_SIDE) {
		return glm::vec2(TEXTURE_TILE_SIZE * 3, TEXTURE_TILE_SIZE * 15);
	}
	else if (type == STONE) {
		return glm::vec2(TEXTURE_TILE_SIZE * 1, TEXTURE_TILE_SIZE * 15);
	}
	else if (type == DIRT) {
		return glm::vec2(TEXTURE_TILE_SIZE * 2, TEXTURE_TILE_SIZE * 15);
	}
	else if (type == BEDROCK) {
		return glm::vec2(TEXTURE_TILE_SIZE * 1, TEXTURE_TILE_SIZE * 14);
	}
	return glm::vec2(0, 15 * TEXTURE_TILE_SIZE);
}

void Chunk::buildTopFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 1, 0) + pos, glm::vec3(0, 1, 0) + pos,
		glm::vec3(1, 1, -1) + pos, glm::vec3(0, 1, -1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(1 + indOffset, 0 + indOffset, 2 + indOffset),
		glm::u32vec3(1 + indOffset, 2 + indOffset, 3 + indOffset)
	};

	glm::vec2 tex = getTexturePosition(face.type);

	std::array<glm::vec2, 4> texture = {
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, tex.y),
		glm::vec2(                    tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, tex.y + TEXTURE_TILE_SIZE),
		glm::vec2(                    tex.x, tex.y + TEXTURE_TILE_SIZE),
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

void Chunk::buildBotFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 0, 0) + pos, glm::vec3(0, 0, 0) + pos,
		glm::vec3(1, 0, -1) + pos, glm::vec3(0, 0, -1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(1 + indOffset, 2 + indOffset, 0 + indOffset),
		glm::u32vec3(1 + indOffset, 3 + indOffset, 2 + indOffset)
	};

	glm::vec2 tex = getTexturePosition(face.type);

	std::array<glm::vec2, 4> texture = {
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, tex.y),
		glm::vec2(tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, TEXTURE_TILE_SIZE + tex.y),
		glm::vec2(tex.x, TEXTURE_TILE_SIZE + tex.y),
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

void Chunk::buildFrontFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, 0) + pos, glm::vec3(1, 0, 0) + pos,
		glm::vec3(1, 1, 0) + pos, glm::vec3(0, 1, 0) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 1 + indOffset, 2 + indOffset),
		glm::u32vec3(0 + indOffset, 2 + indOffset, 3 + indOffset)
	};

	glm::vec2 tex = getTexturePosition(face.type);

	std::array<glm::vec2, 4> texture = {
		glm::vec2(tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, TEXTURE_TILE_SIZE + tex.y),
		glm::vec2(tex.x, TEXTURE_TILE_SIZE + tex.y),
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

void Chunk::buildBackFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, -1) + pos, glm::vec3(1, 0, -1) + pos,
		glm::vec3(1, 1, -1) + pos, glm::vec3(0, 1, -1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 2 + indOffset, 1 + indOffset),
		glm::u32vec3(0 + indOffset, 3 + indOffset, 2 + indOffset)
	};

	glm::vec2 tex = getTexturePosition(face.type);

	std::array<glm::vec2, 4> texture = {
		glm::vec2(tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, TEXTURE_TILE_SIZE + tex.y),
		glm::vec2(tex.x, TEXTURE_TILE_SIZE + tex.y),
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

void Chunk::buildRightFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 0, 0) + pos, glm::vec3(1, 0, -1) + pos,
		glm::vec3(1, 1, -1) + pos, glm::vec3(1, 1, 0) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 1 + indOffset, 2 + indOffset),
		glm::u32vec3(0 + indOffset, 2 + indOffset, 3 + indOffset)
	};

	glm::vec2 tex = getTexturePosition(face.type);

	std::array<glm::vec2, 4> texture = {
		glm::vec2(tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, TEXTURE_TILE_SIZE + tex.y),
		glm::vec2(tex.x, TEXTURE_TILE_SIZE + tex.y),
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

void Chunk::buildLeftFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, 0) + pos, glm::vec3(0, 0, -1) + pos,
		glm::vec3(0, 1, -1) + pos, glm::vec3(0, 1, 0) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 2 + indOffset, 1 + indOffset),
		glm::u32vec3(0 + indOffset, 3 + indOffset, 2 + indOffset)
	};

	glm::vec2 tex = getTexturePosition(face.type);

	std::array<glm::vec2, 4> texture = {
		glm::vec2(tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, tex.y),
		glm::vec2(TEXTURE_TILE_SIZE + tex.x, TEXTURE_TILE_SIZE + tex.y),
		glm::vec2(tex.x, TEXTURE_TILE_SIZE + tex.y),
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

Mesh Chunk::buildChunkFaces(glm::vec2 chunkPos, std::vector<Face> faces)
{
	Mesh mesh;
	std::size_t nVert = 0;
	glm::vec3 worldPos = glm::vec3(chunkPos.x, 0, chunkPos.y);

	setPosition(glm::vec3(chunkPos.x, 0, chunkPos.y));

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
