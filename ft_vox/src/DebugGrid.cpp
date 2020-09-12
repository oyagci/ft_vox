#include "DebugGrid.hpp"
#include "Game.hpp"

DebugGrid::DebugGrid()
{
	static const float vertices[] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
	};

	glGenVertexArrays(1, &_Vao);
	glBindVertexArray(_Vao);

	glGenBuffers(1, &_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)0);

	glBindVertexArray(0);

	_Shader.addVertexShader("shaders/line.vs.glsl")
		.addFragmentShader("shaders/line.fs.glsl")
		.link();
}

void DebugGrid::draw(glm::vec3 const &position, float size, float borderSize)
{
	//glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthFunc(GL_ALWAYS);

	_Shader.bind();
	_Shader.setUniform4x4f("viewProjectionMatrix", Game::get().getWorld().getCamera().getViewProjectionMatrix());

	glm::mat4 model(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(size, borderSize, borderSize));

	_Shader.setUniform4x4f("modelMatrix", model);
	_Shader.setUniform3f("lineColor", { 1.0, 0.0f, 0.0f });

	glBindVertexArray(_Vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(borderSize, size, borderSize));

	_Shader.setUniform4x4f("modelMatrix", model);
	_Shader.setUniform3f("lineColor", { 0.0, 1.0f, 0.0f });

	glDrawArrays(GL_TRIANGLES, 0, 36);


	model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, glm::vec3(borderSize, borderSize, size));

	_Shader.setUniform4x4f("modelMatrix", model);
	_Shader.setUniform3f("lineColor", { 0.0, 0.0f, 1.0f });

	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	_Shader.unbind();

	//glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
}