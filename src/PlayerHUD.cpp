#include "PlayerHUD.hpp"
#include "stb_image.h"

PlayerHUD::PlayerHUD()
{
	// Shader
	_shader.addFragmentShader("shaders/crosshair.fs.glsl")
		.addVertexShader("shaders/crosshair.vs.glsl")
		.link();
	_shader.bind();

	glm::mat4 proj = glm::ortho(0.0f, 2560.0f, 0.0f, 1440.0f);
	_shader.setUniform4x4f("projectionMatrix", proj);
	_shader.setUniform4x4f("modelMatrix", glm::mat4(1.0f));
	_shader.setUniform3f("textColor", glm::vec3(1.0f));

	// Texture
	glGenTextures(1, &_crosshairTexture);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, _crosshairTexture);

	int width, height, nchannel;
	stbi_uc *texture = stbi_load("img/crosshair.png", &width, &height, &nchannel, 0);

	if (!texture) {
		std::cout << "WARNING::PLAYERHUD::CONSTRUCTOR::IMAGE_NOT_FOUND" << std::endl;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	stbi_image_free(texture);

	float crossSize = 18.0f;

	// Mesh
	std::array<glm::vec3, 4> verts = {
		glm::vec3( -(crossSize / 2.0f), -(crossSize / 2.0f), 0.0f ),
		glm::vec3(  (crossSize / 2.0f), -(crossSize / 2.0f), 0.0f ),
		glm::vec3(  (crossSize / 2.0f),  (crossSize / 2.0f), 0.0f ),
		glm::vec3( -(crossSize / 2.0f),  (crossSize / 2.0f), 0.0f ),
	};

	std::array<glm::vec3, 4> tex = {
		glm::vec3(  0.0f,   0.0f, 0.0f ),
		glm::vec3(  1.0f,   0.0f, 0.0f ),
		glm::vec3(  1.0f,   1.0f, 0.0f ),
		glm::vec3(  0.0f,   1.0f, 0.0f ),
	};

	for (auto const &v : verts) {
		_crosshairMesh.addPosition(v + glm::vec3((2560.0f / 2.0f), (1440.0f / 2.0f), 0.0f));
	}
	for (auto const &t : tex) {
		_crosshairMesh.addUv(t);
	}

	_crosshairMesh.addTriangle(glm::uvec3(0, 1, 2))
				  .addTriangle(glm::uvec3(0, 2, 3));
	_crosshairMesh.build();
}

PlayerHUD::~PlayerHUD()
{
	glDeleteTextures(1, &_crosshairTexture);
}

void PlayerHUD::draw()
{
	_shader.bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _crosshairTexture);
	_crosshairMesh.draw();
}
