#include "Cube.hpp"

using namespace lazy::graphics;
using namespace glm;

Cube::Cube(Camera *c, float x, float y, float z)
	: Cube(c, std::move(glm::vec3(x, y, z)))
{
}

Cube::Cube(Camera *c, glm::vec3 pos)
{
	vec3 vertices[] = {
		vec3(0, 0,  0), vec3(1, 0,  0), vec3(1, 1,  0), vec3(0, 1,  0),
		vec3(0, 0, -1), vec3(1, 0, -1), vec3(1, 1, -1), vec3(0, 1, -1),
	};
	vec3 triangles[] = {
		vec3(0, 1, 2), vec3(0, 2, 3), // Front
		vec3(6, 5, 4), vec3(7, 6, 4), // Back
		vec3(0, 3, 7), vec3(0, 7, 4), // Left
		vec3(1, 5, 6), vec3(1, 6, 2), // Right
		vec3(3, 2, 6), vec3(3, 6, 7), // Top
		vec3(0, 5, 1), vec3(0, 4, 5), // Bottom
	};

	_camera = c;

	_position = std::move(pos);
	_mesh = Mesh();
	for (auto &vert : vertices) {
		_mesh.addPosition(vert);
	}
	for (auto &tri : triangles) {
		_mesh.addTriangle(tri);
	}
	_mesh.build();

	_shader.addVertexShader("shaders/basic.vs.glsl")
		   .addFragmentShader("shaders/basic.fs.glsl");
	_shader.link();

	_shader.bind();
	_shader.setUniform4x4f("projectionMatrix",
		_camera->getProjectionMatrix());
	_shader.setUniform4x4f("viewMatrix",
		_camera->getViewMatrix());
	_shader.setUniform4x4f("viewProjectionMatrix",
		_camera->getViewProjectionMatrix());
	_shader.unbind();

}

void Cube::onDraw()
{
	glm::mat4 model(1.0f);

	model = glm::translate(model, _position);

	_shader.bind();
	_shader.setUniform4x4f("modelMatrix", model);
	_mesh.draw();
	_shader.unbind();
}
