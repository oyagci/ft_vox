#pragma once

#include <glm/vec3.hpp>
#include "lazy.hpp"

using namespace lazy::graphics;
using namespace glm;

class Cube
{
public:
	Cube(Camera *c, glm::vec3 pos);
	Cube(Camera *c, float x, float y, float z);

	void onDraw();

private:
	vec3 _position;
	Mesh _mesh;
	Shader _shader;
	Camera *_camera;
};
