#include "./CubePrimitive.hpp";

CubePrimitive::CubePrimitive()
{
    _mesh.addPosition(glm::vec3(-1, -1, -1))
		.addPosition(glm::vec3(-1, 1, -1))
		.addPosition(glm::vec3(1, 1, -1))
		.addPosition(glm::vec3(1, -1, -1))
		.addTriangle(glm::u32vec3(0, 1, 2))
		.addTriangle(glm::u32vec3(0, 2, 3))

		.addPosition(glm::vec3(-1, -1, 1))
		.addPosition(glm::vec3(-1, 1, 1))
		.addPosition(glm::vec3(1, 1, 1))
		.addPosition(glm::vec3(1, -1, 1))
		.addTriangle(glm::u32vec3(4, 6, 5))
		.addTriangle(glm::u32vec3(4, 7, 6))

		.addPosition(glm::vec3(-1, -1, -1))
		.addPosition(glm::vec3(-1, -1, 1))
		.addPosition(glm::vec3(1, -1, 1))
		.addPosition(glm::vec3(1, -1, -1))
		.addTriangle(glm::u32vec3(8, 10, 9))
		.addTriangle(glm::u32vec3(8, 11, 10))

		.addPosition(glm::vec3(-1, 1, -1))
		.addPosition(glm::vec3(-1, 1, 1))
		.addPosition(glm::vec3(1, 1, 1))
		.addPosition(glm::vec3(1, 1, -1))
		.addTriangle(glm::u32vec3(12, 13, 14))
		.addTriangle(glm::u32vec3(12, 14, 15))

		.addPosition(glm::vec3(-1, -1, -1))
		.addPosition(glm::vec3(-1, -1, 1))
		.addPosition(glm::vec3(-1, 1, 1))
		.addPosition(glm::vec3(-1, 1, -1))
		.addTriangle(glm::u32vec3(16, 17, 18))
		.addTriangle(glm::u32vec3(16, 18, 19))

		.addPosition(glm::vec3(1, -1, -1))
		.addPosition(glm::vec3(1, -1, 1))
		.addPosition(glm::vec3(1, 1, 1))
		.addPosition(glm::vec3(1, 1, -1))
		.addTriangle(glm::u32vec3(20, 22, 21))
		.addTriangle(glm::u32vec3(20, 23, 22));
	_mesh.build();
}

void CubePrimitive::draw()
{
    _mesh.draw();
}