#pragma once

#include "lazy.hpp"

using lazy::graphics::Shader;
using lazy::graphics::Mesh;
using lazy::graphics::Camera;

class PlayerHUD
{
public:
	PlayerHUD();
	~PlayerHUD();
	void draw();

private:
	Shader _shader;
	GLuint _vao;
	GLuint _vbo;
	GLuint _crosshairTexture;
	Mesh _crosshairMesh;
};
