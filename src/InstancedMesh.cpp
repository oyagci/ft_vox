#include "InstancedMesh.hpp"
#include <glm/glm.hpp>
#include <string>

void InstancedMesh::drawInstanced(std::size_t nbInstances,
	std::vector<glm::vec3> &positions)
{

	std::size_t i = 0;
	for (auto &pos : positions) {

		std::string offset = std::string("offsets[");
		offset += std::to_string(i);
		offset += "]";

		_shader->setUniform3f(offset, pos);

		i += 1;
	}

	glBindVertexArray(vao);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT,
		nullptr, nbInstances);
	glBindVertexArray(0);
}
