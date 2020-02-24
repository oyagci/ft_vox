#include "Chunk.hpp"
#include <cstdint>
#include "SimplexNoise.hpp"
#include <mutex>
#include "World.hpp"
#include "Time.hpp"

using lazy::utils::Time;

SimplexNoise s = SimplexNoise(0.1f, 1.0f, 2.0f, 0.25f);

float simplexNoise(size_t octaves, glm::vec3 pos)
{
	return s.fractal(octaves, pos.x, pos.y, pos.z);
}

Chunk::Chunk(unsigned int seed, glm::ivec2 pos, World *world) : _world(world)
{
	_blocks = std::make_unique<std::array<Block, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE>>();
	_position = std::move(pos);
	_verticalOffset = -VERTICAL_OFFSET;
	_offsetTime = 0.0f;
	_state = ChunkState::NOT_GENERATED;

	// Don't animate this chunk if it is too close to the player
	glm::vec3 player = world->getPlayerPosition();
	if (glm::length(glm::vec2(pos) - glm::vec2(player.x, player.z)) < 200.0f) {
		_verticalOffset = 0.0f;
		_offsetTime = 1.0f;
	}

	glm::vec3 seedOffset(seed % 1000000, 0.0f, seed % 1000000);

	for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
		for (std::size_t y = CHUNK_SIZE - 1; y + 1 > 0; y--) {
			for (std::size_t z = 0; z < CHUNK_SIZE; z++) {

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
	genTrees(seedOffset);
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

void Chunk::generate()
{
	auto f = std::async(std::launch::async, &Chunk::genChunkFaces, this);
}

void Chunk::build()
{
	Mesh tmp = buildChunkFaces(_position, _faces);
	tmp.build();
	_mesh = std::move(tmp);
}

void Chunk::draw()
{
	if (_state == ChunkState::BUILT || _state == ChunkState::DONE) {
		_mesh.draw();
	}
}

void Chunk::action(ChunkAction action)
{
	switch (action) {
	case ChunkAction::START_GENERATE:
		if (_state == ChunkState::NOT_GENERATED) {
			_state = ChunkState::IS_GENERATING;
			generate();
			Chunk::action(ChunkAction::END_GENERATE);
		}
		break ;
	case ChunkAction::END_GENERATE:
		if (_state == ChunkState::IS_GENERATING) {
			_state = ChunkState::NOT_BUILT;
		}
		break ;
	case ChunkAction::START_BUILD:
		if (_state == ChunkState::NOT_BUILT) {
			build();
			Chunk::action(ChunkAction::END_BUILD);
		}
		break ;
	case ChunkAction::END_BUILD:
		if (_state == ChunkState::NOT_BUILT) {
			_state = ChunkState::BUILT;
		}
		break ;
	case ChunkAction::SET_DONE:
		if (_state == ChunkState::BUILT) {
			_state = ChunkState::DONE;
		}
		break ;
	case ChunkAction::REGENERATE:
		_state = ChunkState::NOT_GENERATED;
		break ;
	default:
		break ;
	}
}

void Chunk::update()
{
	float deltaTime = Time::getDeltaTime();

	auto cubicBezier = [] (float t, float p0, float p1, float p2, float p3) -> float {
		float res = std::pow(1.0f - t, 3.0f) * p0 +
					3.0f * std::pow(1.0f - t, 2.0f) * t * p1 +
					3.0f * (1.0f - t) * std::pow(t , 2.0f) * p2 +
					std::pow(t, 3.0f) * p3;
		return res;
	};

	switch (_state) {
	case ChunkState::NOT_GENERATED:
		if (getUnavailableSides() == 0) {
			onGenerate();
		}
		break ;
	case ChunkState::NOT_BUILT:
		onBuild();
		break ;
	case ChunkState::BUILT:
		if (_offsetTime <= 1.0f) {
			_verticalOffset = -VERTICAL_OFFSET + (cubicBezier(_offsetTime, 0.0f, 1.0f, 1.0f, 1.0f) * VERTICAL_OFFSET);
			_offsetTime += VERTICAL_OFFSET_STEP * deltaTime;
		}
		else {
			_verticalOffset = 0.0f;
			_state = ChunkState::DONE;
			action(ChunkAction::SET_DONE);
		}
		break;
	default:
		break;
	};
}

int Chunk::getVisibleFaces(int x, int y, int z)
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

auto Chunk::genFaceToRender(glm::vec3 pos, FaceDirection f, Chunk::Block const &block) -> Face
{
	Face face = { pos, f, BlockType::AIR };

	if (block == 1) {
		if (f == FaceDirection::TOP) {
			face.type = BlockType::GRASS;
		}
		else if (f == FaceDirection::LEFT || f == FaceDirection::RIGHT ||
				f == FaceDirection::FRONT || f == FaceDirection::BACK) {
			face.type = BlockType::GRASS_SIDE;
		}
		else if (f == FaceDirection::BOT) {
			face.type = BlockType::DIRT;
		}
	}
	else if (block == 2) {
		face.type = BlockType::STONE;
	}
	else if (block == 3) {
		face.type = BlockType::DIRT;
	}
	else if (block == 4) {
		face.type = BlockType::BEDROCK;
	}
	else if (block == 5) {
		if ((f == FaceDirection::LEFT || f == FaceDirection::RIGHT ||
			 f == FaceDirection::FRONT || f == FaceDirection::BACK)) {
			face.type = BlockType::OAK_LOG;
		}
		else {
			face.type = BlockType::OAK_LOG_TOP;
		}
	}
	else if (block == 6) {
		face.type = BlockType::LEAF;
	}

	return face;
}

void Chunk::genChunkFaces()
{
	std::vector<Face> faces;

	for (size_t x = 0; x < Chunk::CHUNK_SIZE; x++) {
		for (size_t y = 0; y < Chunk::CHUNK_SIZE; y++) {
			for (size_t z = 0; z < Chunk::CHUNK_SIZE; z++) {

				glm::ivec3 worldPos(getPosition().x + x, y, getPosition().y + z);
				Chunk::Block b = getBlock(x, y, z);

				if (!b) {
					continue;
				}

				if (y + 1 >= Chunk::CHUNK_SIZE) {
//					if (!_world->getBlock(worldPos.x, worldPos.y + 1, worldPos.z).has_value()) {
						faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::TOP, b));
//					}
				}
				else if (!getBlock(x, y + 1, z)) {
					faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::TOP, b));
				}

				if (static_cast<int>(y) - 1 < 0) {
//					if (!_world->getBlock(worldPos.x, worldPos.y - 1, worldPos.z).has_value()) {
						faces.push_back(genFaceToRender(std::move(glm::ivec3(x, y, z)), FaceDirection::BOT, b));
//					}
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
	}
	_faces = std::move(faces);
}

glm::vec2 Chunk::getTexturePosition(BlockType type)
{
	if (type == BlockType::GRASS) {
		return glm::vec2(TEXTURE_TILE_SIZE * 11, TEXTURE_TILE_SIZE * 4);
	}
	else if (type == BlockType::GRASS_SIDE) {
		return glm::vec2(TEXTURE_TILE_SIZE * 3, TEXTURE_TILE_SIZE * 15);
	}
	else if (type == BlockType::STONE) {
		return glm::vec2(TEXTURE_TILE_SIZE * 1, TEXTURE_TILE_SIZE * 15);
	}
	else if (type == BlockType::DIRT) {
		return glm::vec2(TEXTURE_TILE_SIZE * 2, TEXTURE_TILE_SIZE * 15);
	}
	else if (type == BlockType::BEDROCK) {
		return glm::vec2(TEXTURE_TILE_SIZE * 1, TEXTURE_TILE_SIZE * 14);
	}
	else if (type == BlockType::OAK_LOG) {
		return glm::vec2(TEXTURE_TILE_SIZE * 4, TEXTURE_TILE_SIZE * 14);
	}
	else if (type == BlockType::OAK_LOG_TOP) {
		return glm::vec2(TEXTURE_TILE_SIZE * 5, TEXTURE_TILE_SIZE * 14);
	}
	else if (type == BlockType::LEAF) {
		return glm::vec2(TEXTURE_TILE_SIZE * 12, TEXTURE_TILE_SIZE * 4);
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

unsigned int Chunk::getUnavailableSides()
{
	glm::ivec2 gridPos(_position / CHUNK_SIZE);

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

void Chunk::genTrees(glm::vec3 seedOff)
{
	for (std::size_t x = 0; x < CHUNK_SIZE; x++) {
		for (std::size_t z = 0; z < CHUNK_SIZE; z++) {

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

void Chunk::putTree(glm::ivec3 root)
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
