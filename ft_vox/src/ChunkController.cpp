#include "ChunkController.hpp"
#include "World.hpp"
#include "Time.hpp"
#include <unordered_map>
#include "FastNoise.h"

using lazy::utils::Time;

FastNoise fn = FastNoise(1337);

float simplexNoise(int octaves, glm::vec3 pos)
{
	fn.SetFractalOctaves(octaves);
	fn.SetFrequency(0.1);
	fn.SetFractalLacunarity(2.0f);

	return fn.GetSimplex(pos.x, pos.y, pos.z);
}

ChunkController::ChunkController(unsigned int seed, glm::ivec2 pos, World *world)
	: _world(world), _data(seed, pos + glm::ivec2(32, 32)) // octree expects position at the center of the chunk
{
	_position = pos;
	_verticalOffset = -VERTICAL_OFFSET;
	_offsetTime = 0.0f;
	_state = std::make_unique<ChunkControllerState_NotGenerated>(this);
	_seed = seed;
	genChunkData();
}

auto ChunkController::getBlock(std::size_t x, std::size_t y, std::size_t z) const -> Block
{
	return _data.getBlock(x, y, z);
}

void ChunkController::setBlock(size_t x, size_t y, size_t z, Block val)
{
	return _data.setBlock(x, y, z, val);
}

void ChunkController::generate(size_t step)
{
	genChunkFaces(step);
	if (step >= Chunk::CHUNK_SIZE - 1)
		_isGenerated = true;
}

void ChunkController::build()
{
	Mesh tmp = buildChunkFaces(_position, _faces);
	_faces.clear();
	tmp.build();
	_mesh = std::move(tmp);
}

void ChunkController::draw()
{
	_mesh.draw();
}

void ChunkController::update()
{
	auto nextState = _state->OnUpdate();

	if (nextState) {
		_state.reset(*nextState);
	}
}

void ChunkController::genChunkData()
{
	glm::vec3 seedOffset(_seed % 1000000, 0.0f, _seed % 1000000);

	for (std::size_t x = 0; x < Chunk::CHUNK_SIZE; x++) {
		for (std::size_t y = Chunk::CHUNK_SIZE - 1; y + 1 > 0; y--) {
			for (std::size_t z = 0; z < Chunk::CHUNK_SIZE; z++) {

				if (y == 0) {
					setBlock(x, y, z, 4);
					continue ;
				}

				glm::vec3 pos = glm::vec3(_position.x, -32, _position.y) + glm::vec3(x, y, z) + seedOffset;

				float result = 0.0f;

				float hH = simplexNoise(1, pos / 10.0f);
				for (int i = 2; i < 3; i++) {
					hH += simplexNoise(1, pos / ((float)i * 5.0f)) * (float)i / 10.0f;
				}

				float pH = simplexNoise(1, pos / 10.0f);
				for (int i = 2; i < 6; i++) {
					pH -= simplexNoise(1, pos / (float)i) * (float)i / 16.0f;
				}

				glm::vec3 mP = glm::vec3(pos.x + 3000.0f, 0, pos.z - 400) / 50.0f;
				float mm = simplexNoise(1, mP);

				glm::vec3 dP = glm::vec3((float)pos.x - 3.4f, 827.0f, pos.z + 51.3f);
				float path = simplexNoise(1, dP);

				for (int i = 2; i < 6; i++) {
					path *= simplexNoise(1, dP / (float)i) * (float)i / 6.0f;
				}

				glm::vec3 cP = glm::vec3((float)pos.x / 2.0f - 523.432, (float)pos.y - 2.827, (float)pos.z / 2.0f + 5516.0f);
				float cave = simplexNoise(1, cP / 5.0f);
				for (int i = 0; i < 6; i++) {
					cave += simplexNoise(1, cP / ((float)i * 2.0f)) * (float)i / 6.0f;
				}

				if (abs(floor(cave * 50.0f)) - 30.0f > 0.0f) {
					cave = abs(cave * 10.0f);
				}
				else {
					cave = 0;
				}

				result = -pos.y + (pH / 2 + hH) * (Chunk::CHUNK_SIZE / 2) * mm + path * 10 - cave * abs(mm);

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

	genTrees(seedOffset);
}

void ChunkController::genTrees(glm::vec3 seedOff)
{
	for (std::size_t x = 0; x < Chunk::CHUNK_SIZE; x++) {
		for (std::size_t z = 0; z < Chunk::CHUNK_SIZE; z++) {

			glm::ivec3 pos = glm::vec3(_position.x + 100, 0, _position.y + 100) + glm::vec3(x, 0, z) + seedOff;

			float n = (simplexNoise(4, glm::vec3(pos.x / 10.0f, 0.0f, pos.z / 10.0f)) + 1.0f) / 2.0f;
			float n2 = (simplexNoise(10, glm::vec3(pos.x * 10.0f, 0.0f, pos.z * 10.0f)) + 1.0f) / 2.0f;

			float val = n2 - (0.95f - n);

			if (val > 0.7f) {
				putTree(glm::vec3(x, 0, z));
			}
		}
	}
}

void ChunkController::putTree(glm::ivec3 root)
{
	int treeShape[6][5][5] = {
		{
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 6, 0, 0 },
			{ 0, 6, 6, 6, 0 },
			{ 0, 0, 6, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 0, 0, 0 },
			{ 0, 6, 6, 6, 0 },
			{ 0, 6, 5, 6, 0 },
			{ 0, 6, 6, 6, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 5, 6, 6 },
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 6, 6, 6 },
		},
		{
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 5, 6, 6 },
			{ 6, 6, 6, 6, 6 },
			{ 6, 6, 6, 6, 6 },
		},
		{
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 5, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
		{
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 5, 0, 0 },
			{ 0, 0, 0, 0, 0 },
			{ 0, 0, 0, 0, 0 },
		},
	};
	for (size_t y = Chunk::CHUNK_SIZE - 4; y > 0; y--) {
		if (getBlock(root.x, y, root.z) == 1) {
			for (int yTree = 0; yTree < 6; yTree++) {
				for (int xTree = 0; xTree < 5; xTree++) {
					for (int zTree = 0; zTree < 5; zTree++) {
						if (treeShape[yTree][xTree][zTree] != 0) {
							setBlock(
								root.x + xTree - 2,
								y + 1 + (5 - yTree),
								root.z + zTree - 2,
								treeShape[yTree][xTree][zTree]);
						}
					}
				}
			}
		}
	}
}

int ChunkController::getVisibleFaces(int x, int y, int z)
{
	int result = 0;

	bool top;
	bool bottom;
	bool left;
	bool right;
	bool front;
	bool back;

	auto blockIsPresent = [this] (int x, int y, int z) -> bool {
		auto b = _world->getBlock(x, y, z);
		return b.has_value() && b.value() != 0;
	};

	top = !blockIsPresent(x, y + 1, z);
	bottom = !blockIsPresent(x, y - 1, z);
	left = !blockIsPresent(x - 1, y, z);
	right = !blockIsPresent(x + 1, y, z);
	front = !blockIsPresent(x, y, z + 1);
	back = !blockIsPresent(x, y, z - 1);

	result |= top    ? 0 : 1 << 0;
	result |= bottom ? 0 : 1 << 1;
	result |= left   ? 0 : 1 << 2;
	result |= right  ? 0 : 1 << 3;
	result |= front  ? 0 : 1 << 4;
	result |= back   ? 0 : 1 << 5;

	return result;
}

auto ChunkController::genFaceToRender(glm::vec3 pos, FaceDirection f, ChunkController::Block const &block) -> Face
{
	Face face = { pos, f, Chunk::Chunk::BlockType::AIR };

	if (block == 1) {
		if (f == FaceDirection::TOP) {
			face.type = Chunk::BlockType::GRASS;
		}
		else if (f == FaceDirection::LEFT || f == FaceDirection::RIGHT ||
				f == FaceDirection::FRONT || f == FaceDirection::BACK) {
			face.type = Chunk::BlockType::GRASS_SIDE;
		}
		else if (f == FaceDirection::BOT) {
			face.type = Chunk::BlockType::DIRT;
		}
	}
	else if (block == 2) {
		face.type = Chunk::BlockType::STONE;
	}
	else if (block == 3) {
		face.type = Chunk::BlockType::DIRT;
	}
	else if (block == 4) {
		face.type = Chunk::BlockType::BEDROCK;
	}
	else if (block == 5) {
		if ((f == FaceDirection::LEFT || f == FaceDirection::RIGHT ||
			 f == FaceDirection::FRONT || f == FaceDirection::BACK)) {
			face.type = Chunk::BlockType::OAK_LOG;
		}
		else {
			face.type = Chunk::BlockType::OAK_LOG_TOP;
		}
	}
	else if (block == 6) {
		face.type = Chunk::BlockType::LEAF;
	}

	return face;
}

void ChunkController::genChunkFaces(size_t step)
{
	std::vector<Face> faces;

	size_t x = step;
	for (size_t y = 0; y < Chunk::CHUNK_SIZE; y++) {
		for (size_t z = 0; z < Chunk::CHUNK_SIZE; z++) {

			glm::ivec3 worldPos(getPosition().x + x, y, getPosition().y + z);
			Chunk::Block b = getBlock(x, y, z);

			if (!b) {
				continue;
			}

			if (y + 1 >= Chunk::CHUNK_SIZE) {
//				if (!_world->getBlock(worldPos.x, worldPos.y + 1, worldPos.z).has_value()) {
					faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::TOP, b));
//				}
			}
			else if (!getBlock(x, y + 1, z)) {
				faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::TOP, b));
			}

			if (static_cast<int>(y) - 1 < 0) {
//				if (!_world->getBlock(worldPos.x, worldPos.y - 1, worldPos.z).has_value()) {
					faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::BOT, b));
//				}
			}
			else if (!getBlock(x, y - 1, z)) {
				faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::BOT, b));
			}

			if (x + 1 >= Chunk::CHUNK_SIZE) {
				if (!_world->getBlock(worldPos.x + 1, worldPos.y, worldPos.z).has_value()) {
					faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::RIGHT, b));
				}
			}
			else if (!getBlock(x + 1, y, z)) {
				faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::RIGHT, b));
			}

			if (static_cast<int>(x) - 1 < 0) {
				if (!_world->getBlock(worldPos.x - 1, worldPos.y, worldPos.z).has_value()) {
					faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::LEFT, b));
				}
			}
			else if (!getBlock(x - 1, y, z)) {
				faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::LEFT, b));
			}

			if (z + 1 >= Chunk::CHUNK_SIZE) {
				if (!_world->getBlock(worldPos.x, worldPos.y, worldPos.z + 1).has_value()) {
					faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::FRONT, b));
				}
			}
			else if (!getBlock(x, y, z + 1)) {
				faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::FRONT, b));
			}

			if (static_cast<int>(z) - 1 < 0) {
				if (!_world->getBlock(worldPos.x, worldPos.y, worldPos.z - 1).has_value()) {
					faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::BACK, b));
				}
			}
			else if (!getBlock(x, y, z - 1)) {
				faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::BACK, b));
			}
		}
	}

	_faces.insert(_faces.end(), faces.begin(), faces.end());
}

glm::vec2 ChunkController::getTexturePosition(Chunk::BlockType type)
{
	static const std::unordered_map<Chunk::BlockType, glm::vec2> blocks = {
		{ Chunk::BlockType::GRASS,       glm::vec2(TEXTURE_TILE_SIZE * 11, TEXTURE_TILE_SIZE * 4) },
		{ Chunk::BlockType::GRASS_SIDE,  glm::vec2(TEXTURE_TILE_SIZE * 3, TEXTURE_TILE_SIZE * 15) },
		{ Chunk::BlockType::STONE,       glm::vec2(TEXTURE_TILE_SIZE * 1, TEXTURE_TILE_SIZE * 15) },
		{ Chunk::BlockType::DIRT,        glm::vec2(TEXTURE_TILE_SIZE * 2, TEXTURE_TILE_SIZE * 15) },
		{ Chunk::BlockType::BEDROCK,     glm::vec2(TEXTURE_TILE_SIZE * 1, TEXTURE_TILE_SIZE * 14) },
		{ Chunk::BlockType::OAK_LOG,     glm::vec2(TEXTURE_TILE_SIZE * 4, TEXTURE_TILE_SIZE * 14) },
		{ Chunk::BlockType::OAK_LOG_TOP, glm::vec2(TEXTURE_TILE_SIZE * 5, TEXTURE_TILE_SIZE * 14) },
		{ Chunk::BlockType::LEAF,        glm::vec2(TEXTURE_TILE_SIZE * 12, TEXTURE_TILE_SIZE * 4) },
	};

	auto block = blocks.find(type);
	if (block != blocks.end()) {
		return block->second;
	}

	return glm::vec2(0, 15 * TEXTURE_TILE_SIZE);
}

void ChunkController::buildTopFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 1, 0) + pos, glm::vec3(0, 1, 0) + pos,
		glm::vec3(1, 1, 1) + pos, glm::vec3(0, 1, 1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 1 + indOffset, 2 + indOffset),
		glm::u32vec3(1 + indOffset, 3 + indOffset, 2 + indOffset)
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

void ChunkController::buildBotFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 0, 0) + pos, glm::vec3(0, 0, 0) + pos,
		glm::vec3(1, 0, 1) + pos, glm::vec3(0, 0, 1) + pos
	};
	std::array<glm::u32vec3, 2> triangles = {
		glm::u32vec3(0 + indOffset, 2 + indOffset, 1 + indOffset),
		glm::u32vec3(1 + indOffset, 2 + indOffset, 3 + indOffset)
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

void ChunkController::buildFrontFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, 1) + pos, glm::vec3(1, 0, 1) + pos,
		glm::vec3(1, 1, 1) + pos, glm::vec3(0, 1, 1) + pos
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

void ChunkController::buildBackFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, 0) + pos, glm::vec3(1, 0, 0) + pos,
		glm::vec3(1, 1, 0) + pos, glm::vec3(0, 1, 0) + pos
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

void ChunkController::buildRightFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(1, 0, 1) + pos, glm::vec3(1, 0, 0) + pos,
		glm::vec3(1, 1, 0) + pos, glm::vec3(1, 1, 1) + pos
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

void ChunkController::buildLeftFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
{
	std::array<glm::vec3, 4> vertices = {
		glm::vec3(0, 0, 1) + pos, glm::vec3(0, 0, 0) + pos,
		glm::vec3(0, 1, 0) + pos, glm::vec3(0, 1, 1) + pos
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

Mesh ChunkController::buildChunkFaces(glm::vec2 chunkPos, std::vector<Face> faces)
{
	Mesh mesh;
	std::size_t nVert = 0;
	glm::vec3 worldPos = glm::vec3(chunkPos.x, 0, chunkPos.y);

	for (auto &f : faces) {
		switch (f.dir) {
		case FaceDirection::TOP:
			buildTopFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FaceDirection::BOT:
			buildBotFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FaceDirection::FRONT:
			buildFrontFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FaceDirection::BACK:
			buildBackFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FaceDirection::RIGHT:
			buildRightFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
			break ;
		case FaceDirection::LEFT:
			buildLeftFace(f, mesh, f.pos + worldPos, nVert);
			nVert += 4;
		default:
			break;
		}
	}

	return mesh;
}

unsigned int ChunkController::getUnavailableSides()
{
	glm::ivec2 gridPos(_position / Chunk::CHUNK_SIZE);

	auto right = _world->getChunk(glm::ivec2(gridPos.x + 1, gridPos.y)).has_value();
	auto left = _world->getChunk(glm::ivec2(gridPos.x - 1, gridPos.y)).has_value();
	auto front = _world->getChunk(glm::ivec2(gridPos.x,     gridPos.y + 1)).has_value();
	auto back = _world->getChunk(glm::ivec2(gridPos.x,     gridPos.y - 1)).has_value();

	unsigned int unavailable = 0;

	if (!right) {
		unavailable |= static_cast<unsigned int>(FaceDirection::RIGHT);
	}
	if (!left) {
		unavailable |= static_cast<unsigned int>(FaceDirection::LEFT);
	}
	if (!front) {
		unavailable |= static_cast<unsigned int>(FaceDirection::FRONT);
	}
	if (!back) {
		unavailable |= static_cast<unsigned int>(FaceDirection::BACK);
	}

	return unavailable;
}

// Chunk Controller State
// ======================

std::optional<ChunkControllerState*> ChunkControllerState_NotGenerated::OnUpdate()
{
	if (_Controller->getUnavailableSides() == 0) {
		_Controller->generate(_Step++);
		if (_Step >= Chunk::CHUNK_SIZE) {
			return std::make_optional(new ChunkControllerState_IsGenerating(_Controller));
		}
	}
	return std::nullopt;
}

std::optional<ChunkControllerState*> ChunkControllerState_IsGenerating::OnUpdate()
{
	if (_Controller->_isGenerated == true) {
		return std::make_optional(new ChunkControllerState_NotBuilt(_Controller));
	}
	return std::nullopt;
}

std::optional<ChunkControllerState*> ChunkControllerState_NotBuilt::OnUpdate()
{
	_Controller->build();

	return std::make_optional(new ChunkControllerState_Built(_Controller));
}

std::optional<ChunkControllerState*> ChunkControllerState_Built::OnDraw()
{
	_Controller->draw();
	return std::nullopt;
}

std::optional<ChunkControllerState*> ChunkControllerState_Built::OnUpdate()
{
	float deltaTime = lazy::utils::Time::getDeltaTime();

	auto cubicBezier = [] (float t, float p0, float p1, float p2, float p3) -> float {
		float res = std::pow(1.0f - t, 3.0f) * p0 +
					3.0f * std::pow(1.0f - t, 2.0f) * t * p1 +
					3.0f * (1.0f - t) * std::pow(t , 2.0f) * p2 +
					std::pow(t, 3.0f) * p3;
		return res;
	};

	if (_Controller->_offsetTime <= 1.0f) {
		_Controller->_verticalOffset = -ChunkController::VERTICAL_OFFSET + (cubicBezier(_Controller->_offsetTime, 0.0f, 1.0f, 1.0f, 1.0f) * _Controller->VERTICAL_OFFSET);
		_Controller->_offsetTime += ChunkController::VERTICAL_OFFSET_STEP * deltaTime;
	}
	else {
		_Controller->_verticalOffset = 0.0f;

		return std::make_optional(new ChunkControllerState_Done(_Controller));
	}

	return std::nullopt;
}

std::optional<ChunkControllerState*> ChunkControllerState_Done::OnDraw()
{
	_Controller->draw();
	return std::nullopt;
}

std::optional<ChunkControllerState*> ChunkControllerState_Done::OnRegenerate()
{
	return std::make_optional(new ChunkControllerState_NotGenerated(_Controller));
}

std::optional<ChunkControllerState*> ChunkControllerState_Done::OnChangeBlock(glm::ivec3 const &pos, Chunk::Block type)
{
	_Controller->setBlock(pos.x, pos.y, pos.z, type);
	return std::make_optional(new ChunkControllerState_NotGenerated(_Controller));
}


void ChunkController::changeBlock(glm::ivec3 position, Chunk::Block type)
{
	auto nextState = _state->OnChangeBlock(position, type);

	if (nextState) {
		_state.reset(*nextState);
	}
}
