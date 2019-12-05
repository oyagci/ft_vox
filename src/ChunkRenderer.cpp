#include "ChunkRenderer.hpp"
#include <glm/vec3.hpp>

ChunkRenderer::ChunkRenderer() : _chunk(nullptr)
{
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
}

void ChunkRenderer::setChunk(Chunk chunk)
{
	_chunk = std::move(chunk);
}

void ChunkRenderer::onRender()
{
	_mesh.draw();
}


std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
	os << "{ " << v.x << ", " << v.y << ", " << v.z << " }";
	return os;
}

void ChunkRenderer::onUpdateMesh()
{
	_mesh = Mesh();
	std::size_t	nind = 0;

	for (auto &quad : _quads) {
		for (auto &pos : std::get<0>(quad)) {
			_mesh.addPosition(pos);
		}

		quadInds inds = std::get<1>(quad);
		_mesh.addTriangle(glm::u32vec3(nind + inds[0], nind + inds[1], nind + inds[2]))
			 .addTriangle(glm::u32vec3(nind + inds[3], nind + inds[4], nind + inds[5]));

		nind += 3;
	}

	_mesh.build();
}

void ChunkRenderer::onTessellate()
{
	std::vector<quad> quads;

	for (std::size_t h = 0; h < 2; h++) {

		for (std::size_t x = 0; x < 16; x++) {

			for (std::size_t y = 0; y < 1; y++) {

				Chunk::Block b = _chunk.getBlockAt(h, x, y);

				if (b != 0) { continue; }

				glm::vec3 pos = glm::vec3(x, h, y);

				glm::vec3 leftPos  = glm::vec3(pos.x - 0.5f, pos.y, pos.z);
				glm::vec3 rightPos = glm::vec3(pos.x + 0.5f, pos.y, pos.z);
				glm::vec3 frontPos = glm::vec3(pos.x, pos.y, pos.z + 0.5f);
				glm::vec3 backPos  = glm::vec3(pos.x, pos.y, pos.z - 0.5f);
				glm::vec3 botPos   = glm::vec3(pos.x, pos.y - 0.5f, pos.z);
  				glm::vec3 topPos   = glm::vec3(pos.x, pos.y + 0.5f, pos.z);

				quad q = genTopQuad(topPos);
				quads.push_back(std::move(q));

				q = genBotQuad(botPos);
				quads.push_back(std::move(q));
//
//				q = genFrontQuad(frontPos);
//				quads.push_back(std::move(q));
//
//				q = genBackQuad(backPos);
//				quads.push_back(std::move(q));
//
//				q = genLeftQuad(leftPos);
//				quads.push_back(std::move(q));
//
//				q = genRightQuad(rightPos);
//				quads.push_back(std::move(q));

			}
		}
	}

	for (std::size_t h = 0; h < 2; h++) {

		for (std::size_t x = 0; x < 16; x++) {

			for (std::size_t y = 0; y < 1; y++) {

				Chunk::Block b = _chunk.getBlockAt(h, x, y);

				if (b != 0) { continue; }

				glm::vec3 pos = glm::vec3(x, h, y);
				glm::vec3 botPos = glm::vec3(pos.x, pos.y - 0.5f, pos.z);

				quad q = genBotQuad(botPos);
				quads.push_back(std::move(q));
			}
		}
	}

	_quads = std::move(quads);

	onUpdateMesh();
}

ChunkRenderer::quad ChunkRenderer::genTopQuad(glm::vec3 pos)
{
	quadVerts verts = {
		glm::vec3(-0.5f, 0.0f,  0.5f),
		glm::vec3( 0.5f, 0.0f,  0.5f),
		glm::vec3( 0.5f, 0.0f, -0.5f),
		glm::vec3(-0.5f, 0.0f, -0.5f),
	};
	quadInds inds = {
		0, 1, 2,
		0, 2, 3,
	};

	verts[0] += pos;
	verts[1] += pos;
	verts[2] += pos;
	verts[3] += pos;

	return std::make_tuple<quadVerts, quadInds>(std::move(verts), std::move(inds));
}

ChunkRenderer::quad ChunkRenderer::genBotQuad(glm::vec3 pos)
{
	quadVerts verts = {
		glm::vec3(-0.5f, 0.0f,  0.5f),
		glm::vec3( 0.5f, 0.0f,  0.5f),
		glm::vec3( 0.5f, 0.0f, -0.5f),
		glm::vec3(-0.5f, 0.0f, -0.5f),
	};
	quadInds inds = {
		0, 2, 1,
		0, 3, 2,
	};

	verts[0] += pos;
	verts[1] += pos;
	verts[2] += pos;
	verts[3] += pos;

	return std::make_tuple<quadVerts, quadInds>(std::move(verts), std::move(inds));
}

ChunkRenderer::quad ChunkRenderer::genFrontQuad(glm::vec3 pos)
{
	quadVerts verts = {
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3( 0.5f, -0.5f, 0.0f),
		glm::vec3( 0.5f,  0.5f, 0.0f),
		glm::vec3(-0.5f,  0.5f, 0.0f),
	};
	quadInds inds = {
		0, 1, 2,
		0, 2, 3,
	};

	verts[0] += pos;
	verts[1] += pos;
	verts[2] += pos;
	verts[3] += pos;

	return std::make_tuple<quadVerts, quadInds>(std::move(verts), std::move(inds));
}

ChunkRenderer::quad ChunkRenderer::genBackQuad(glm::vec3 pos)
{
	quadVerts verts = {
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3( 0.5f, -0.5f, 0.0f),
		glm::vec3( 0.5f,  0.5f, 0.0f),
		glm::vec3(-0.5f,  0.5f, 0.0f),
	};
	quadInds inds = {
		0, 2, 1,
		0, 3, 2,
	};

	verts[0] += pos;
	verts[1] += pos;
	verts[2] += pos;
	verts[3] += pos;

	return std::make_tuple<quadVerts, quadInds>(std::move(verts), std::move(inds));
}

ChunkRenderer::quad ChunkRenderer::genLeftQuad(glm::vec3 pos)
{
	quadVerts verts = {
		glm::vec3(-0.5f, -0.5f,  0.0f),
		glm::vec3( 0.5f, -0.5f,  0.0f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
	};
	quadInds inds = {
		0, 1, 2,
		0, 2, 3,
	};

	verts[0] += pos;
	verts[1] += pos;
	verts[2] += pos;
	verts[3] += pos;

	return std::make_tuple<quadVerts, quadInds>(std::move(verts), std::move(inds));
}

ChunkRenderer::quad ChunkRenderer::genRightQuad(glm::vec3 pos)
{
	quadVerts verts = {
		glm::vec3(-0.5f, -0.5f,  0.0f),
		glm::vec3( 0.5f, -0.5f,  0.0f),
		glm::vec3(-0.5f,  0.5f, -0.5f),
		glm::vec3(-0.5f, -0.5f, -0.5f),
	};
	quadInds inds = {
		0, 2, 1,
		0, 2, 3,
	};

	verts[0] += pos;
	verts[1] += pos;
	verts[2] += pos;
	verts[3] += pos;

	return std::make_tuple<quadVerts, quadInds>(std::move(verts), std::move(inds));
}
