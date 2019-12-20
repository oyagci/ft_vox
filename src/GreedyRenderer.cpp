#include "GreedyRenderer.hpp"
#include <cstring>
#include <iostream>

using std::size_t;

std::ostream& operator<<(std::ostream &os, glm::vec3 rhs)
{
	os << "{ " << rhs.x << ", " << rhs.y << ", " << rhs.z << " }";
	return os;
}

GreedyRenderer::GreedyRenderer()
{
}

GreedyRenderer::~GreedyRenderer()
{
}

std::vector<VoxelQuad> GreedyRenderer::greedy(std::shared_ptr<Chunk> chunk)
{
	glm::vec3 chunkPos = glm::vec3(chunk->getPos().x, 0, chunk->getPos().y);
	std::vector<VoxelQuad> vquads;
	int chunkSize = static_cast<int>(Chunk::CHUNK_SIZE);

	for (int d = 0; d < 3; ++d)
	{
		int i;
		int j;
		int k;
		int l;
		int w;
		int h;

		int u = (d + 1) % 3;
		int v = (d + 2) % 3;

		int x[3] = { 0, 0, 0 };
		int q[3] = { 0, 0, 0 };

		int *mask = new int[chunkSize * chunkSize];

		q[d] = 1;

		for (x[d] = -1; x[d] < chunkSize;)
		{
			int n = 0;
			for (x[v] = 0; x[v] < chunkSize; ++x[v])
			{
				for (x[u] = 0; x[u] < chunkSize; ++x[u])
				{
					bool blockCur = 0 <= x[d]            ? chunk->getBlock(x[0], x[1], x[2]) : false;
					bool blockCmp = x[d] < chunkSize - 1 ? chunk->getBlock(x[0] + q[0], x[1] + q[1], x[2] + q[2]) : false;
					mask[n++] = blockCur != blockCmp;
				}
			}
			++x[d];
			n = 0;
			for (j = 0; j < chunkSize; j++) {
				for (i = 0; i < chunkSize;) {
					if (mask[n]) {
						for (w = 1; i + w < chunkSize && mask[n + w]; w++) {}

						bool done = false;
						for (h = 1; j + h < chunkSize; h++) {
							for (k = 0; k < w; ++k) {
								if (!mask[n + k + h * chunkSize]) {
									done = true;
									break ;
								}
							}
							if (done) {
								break ;
							}
						}

						x[u] = i;
						x[v] = j;

						int du[3] = { 0, 0, 0 };
						du[u] = w;

						int dv[3] = { 0, 0, 0 };
						dv[v] = h;

						VoxelQuad quad = {
							glm::vec3(x[0] + chunkPos.x,
									  x[1],
									  x[2] + chunkPos.z),

							glm::vec3(x[0] + du[0] + chunkPos.x,
									  x[1] + du[1],
									  x[2] + du[2] + chunkPos.z),

							glm::vec3(x[0] + dv[0] + chunkPos.x,
									  x[1] + dv[1],
									  x[2] + dv[2] + chunkPos.z),

							glm::vec3(x[0] + du[0] + dv[0] + chunkPos.x,
									  x[1] + du[1] + dv[1],
									  x[2] + du[2] + dv[2] + chunkPos.z),
						};
						vquads.push_back(std::move(quad));

						for (l = 0; l < h; l++) {
							for (k = 0; k < w; k++) {
								mask[n + k + l * chunkSize] = false;
							}
						}

						i += w;
						n += w;
					}
					else {
						i++;
						n++;
					}
				}
			}
		}
		delete[] mask;
	}
	return vquads;
}

Mesh GreedyRenderer::generateMesh(std::vector<VoxelQuad> quads)
{
	Mesh mesh;
	std::size_t nVerts = 0;

	for (auto &q : quads) {

		mesh.addPosition(q.botLeft);
		mesh.addPosition(q.botRight);
		mesh.addPosition(q.topRight);
		mesh.addPosition(q.topLeft);

		mesh.addTriangle(glm::u32vec3(0 + nVerts, 2 + nVerts, 1 + nVerts));
		mesh.addTriangle(glm::u32vec3(0 + nVerts, 3 + nVerts, 2 + nVerts));

		nVerts += 4;
	}
	mesh.build();
	return mesh;
}
