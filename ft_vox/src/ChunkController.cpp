#include "ChunkController.hpp"
#include "World.hpp"
#include "Time.hpp"

using lazy::utils::Time;

ChunkController::ChunkController(unsigned int seed, glm::ivec2 pos, World *world)
	: _world(world), _data(seed, pos)
{
	_position = pos;
	_verticalOffset = -VERTICAL_OFFSET;
	_offsetTime = 0.0f;
	_state = std::make_unique<ChunkControllerState_NotGenerated>(this);
}

auto ChunkController::getBlock(std::size_t x, std::size_t y, std::size_t z) const -> Block
{
	return _data.getBlock(x, y, z);
}

void ChunkController::setBlock(size_t x, size_t y, size_t z, Block val)
{
	return _data.setBlock(x, y, z, val);
}

void ChunkController::generate()
{
	_isGenerated.store(false);
	auto f = std::async(std::launch::async, &ChunkController::genChunkFaces, this);
}

void ChunkController::build()
{
	Mesh tmp = buildChunkFaces(_position, _faces);
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

void ChunkController::genChunkFaces()
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

	_isGenerated.store(true);
}

glm::vec2 ChunkController::getTexturePosition(Chunk::BlockType type)
{
	if (type == Chunk::BlockType::GRASS) {
		return glm::vec2(TEXTURE_TILE_SIZE * 11, TEXTURE_TILE_SIZE * 4);
	}
	else if (type == Chunk::BlockType::GRASS_SIDE) {
		return glm::vec2(TEXTURE_TILE_SIZE * 3, TEXTURE_TILE_SIZE * 15);
	}
	else if (type == Chunk::BlockType::STONE) {
		return glm::vec2(TEXTURE_TILE_SIZE * 1, TEXTURE_TILE_SIZE * 15);
	}
	else if (type == Chunk::BlockType::DIRT) {
		return glm::vec2(TEXTURE_TILE_SIZE * 2, TEXTURE_TILE_SIZE * 15);
	}
	else if (type == Chunk::BlockType::BEDROCK) {
		return glm::vec2(TEXTURE_TILE_SIZE * 1, TEXTURE_TILE_SIZE * 14);
	}
	else if (type == Chunk::BlockType::OAK_LOG) {
		return glm::vec2(TEXTURE_TILE_SIZE * 4, TEXTURE_TILE_SIZE * 14);
	}
	else if (type == Chunk::BlockType::OAK_LOG_TOP) {
		return glm::vec2(TEXTURE_TILE_SIZE * 5, TEXTURE_TILE_SIZE * 14);
	}
	else if (type == Chunk::BlockType::LEAF) {
		return glm::vec2(TEXTURE_TILE_SIZE * 12, TEXTURE_TILE_SIZE * 4);
	}
	return glm::vec2(0, 15 * TEXTURE_TILE_SIZE);
}

void ChunkController::buildTopFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkController::buildBotFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkController::buildFrontFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkController::buildBackFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkController::buildRightFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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

void ChunkController::buildLeftFace(Face const &face, Mesh &mesh, glm::vec3 pos, std::size_t indOffset)
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
		_Controller->generate();
		return std::make_optional(new ChunkControllerState_IsGenerating(_Controller));
	}
	return std::nullopt;
}

std::optional<ChunkControllerState*> ChunkControllerState_IsGenerating::OnUpdate()
{
	if (_Controller->_isGenerated.load() == true) {
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

