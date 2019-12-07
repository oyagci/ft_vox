#include "./QuadPrimitive.hpp"

QuadPrimitive::QuadPrimitive()
{
    _mesh.addPosition(glm::vec3(-1, -1, 0))
		.addPosition(glm::vec3(-1, 1, 0))
		.addPosition(glm::vec3(1, 1, 0))
		.addPosition(glm::vec3(1, -1, 0))
		.addTriangle(glm::u32vec3(0, 2, 1))
		.addTriangle(glm::u32vec3(0, 3, 2));

	_mesh.build();
}

void QuadPrimitive::draw()
{
    _mesh.draw();
}